/*
 * Fledge filter Measured values cycling check.
 *
 * Copyright (c) 2020, RTE (https://www.rte-france.com)
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Yannick Marchetaux
 * 
 */
#include "datapoint.h"
#include "reading.h"

#include <filterMvScale.h>
#include <constantsMvScale.h>
#include <datapoint_utility.h>

using namespace std;
using namespace DatapointUtility;

/**
 * Constructor for the LogFilter.
 *
 * We call the constructor of the base class and handle the initial
 * configuration of the filter.
 *
 * @param    filterName      The name of the filter
 * @param    filterConfig    The configuration category for this filter
 * @param    outHandle       The handle of the next filter in the chain
 * @param    output          A function pointer to call to output data to the next filter
 */
FilterMvScale::FilterMvScale(const std::string& filterName,
                        ConfigCategory& filterConfig,
                        OUTPUT_HANDLE *outHandle,
                        OUTPUT_STREAM output) :
                                FledgeFilter(filterName, filterConfig, outHandle, output)
{
}

/**
 * Modification of configuration
 * 
 * @param jsonExchanged : configuration ExchangedData
*/
void FilterMvScale::setJsonConfig(const string& jsonExchanged) {
    m_configPlugin.importExchangedData(jsonExchanged);
}

/**
 * The actual filtering code
 *
 * @param readingSet The reading data to filter
 */
void FilterMvScale::ingest(READINGSET *readingSet) 
{
    lock_guard<mutex> guard(m_configMutex);
	
    // Filter enable, process the readings 
    if (!isEnabled()) {
        (*m_func)(m_data, readingSet);    
        return;
    }

    // Just get all the readings in the readingset
    const std::vector<Reading*>& readings = readingSet->getAllReadings();
    for (auto reading = readings.cbegin(); reading != readings.cend(); reading++) {

        // Get datapoints on readings
        Datapoints& dataPoints = (*reading)->getReadingData();
        string assetName = (*reading)->getAssetName();

        string beforeLog = ConstantsMvScale::NamePlugin + " - " + assetName + " - ingest : ";

        Logger::getLogger()->debug("%s ReceivData %s", beforeLog.c_str(), (*reading)->toJSON().c_str());

        Datapoints *dpPivotTM = findDictElement(&dataPoints, ConstantsMvScale::KeyMessagePivotJsonRoot);
        if (dpPivotTM == nullptr) {
            Logger::getLogger()->debug("%s Missing %s attribute, it is ignored", beforeLog.c_str(), ConstantsMvScale::KeyMessagePivotJsonRoot.c_str());
            continue;
        }

        Datapoints *dpGtim = findDictElement(dpPivotTM, ConstantsMvScale::KeyMessagePivotJsonGt);
        if (dpGtim == nullptr) {
            Logger::getLogger()->debug("%s Missing %s attribute, it is ignored", beforeLog.c_str(), ConstantsMvScale::KeyMessagePivotJsonGt.c_str());
            continue;
        }

        Datapoints *dpTyp = findDictElement(dpGtim, ConstantsMvScale::KeyMessagePivotJsonCdcMv);
        if (dpTyp == nullptr) {
            Logger::getLogger()->debug("%s Missing %s attribute, it is ignored", beforeLog.c_str(), ConstantsMvScale::KeyMessagePivotJsonCdcMv.c_str());
            continue;
        }

        Datapoints *dpMag = findDictElement(dpTyp, ConstantsMvScale::KeyMessagePivotJsonMag);
        if (dpMag == nullptr) {
            Logger::getLogger()->debug("%s Missing %s attribute, it is ignored", beforeLog.c_str(), ConstantsMvScale::KeyMessagePivotJsonMag.c_str());
            continue;
        }

        string id = findStringElement(dpGtim, ConstantsMvScale::KeyMessagePivotJsonId);
        if (id.compare("") == 0) {
            Logger::getLogger()->debug("%s Missing %s attribute, it is ignored", beforeLog.c_str(), ConstantsMvScale::KeyMessagePivotJsonId.c_str());
            continue;
        }

        double valueMv = 0;
        DatapointValue *dpValueMesure = findValueElement(dpMag, ConstantsMvScale::KeyMessagePivotJsonMagI);
        if (dpValueMesure == nullptr) {
            dpValueMesure = findValueElement(dpMag, ConstantsMvScale::KeyMessagePivotJsonMagF);
            if (dpValueMesure == nullptr) {
                Logger::getLogger()->debug("%s Missing %s attributes (f or i), it is ignored", beforeLog.c_str(), ConstantsMvScale::KeyMessagePivotJsonMag.c_str());
                continue;
            }
        }

        if (dpValueMesure->getType() == DatapointValue::T_INTEGER) {
            valueMv = (double)dpValueMesure->toInt();
        }
        else if (dpValueMesure->getType() == DatapointValue::T_FLOAT) {
            valueMv = dpValueMesure->toDouble();
        }
        else {
            Logger::getLogger()->debug("%s bad type measure, it is ignored", beforeLog.c_str());
            continue;
        }

        DataExchangeDefinition dfMv = m_configPlugin.getDataExchangeWithID(id);
        if (dfMv == DataExchangeDefinition()) {
            Logger::getLogger()->debug("%s id (%s) missing from the configuration, it is ignored", beforeLog.c_str(), id.c_str());
            continue;
        }

        Datapoints *dpQ = findDictElement(dpTyp, ConstantsMvScale::KeyMessagePivotJsonQ);
        if (dpQ == nullptr) {
            Logger::getLogger()->debug("%s Missing %s attribute, it is ignored", beforeLog.c_str(), ConstantsMvScale::KeyMessagePivotJsonQ.c_str());
            continue;
        }

        ScaleResult resultScale;
        ScaleMV sc;           
        
        // Check quality
        bool checkValidity = this->checkValidity(dpQ);
        if(checkValidity) {
            Logger::getLogger()->debug("%s check validity success", beforeLog.c_str());                
            valueMv = sc.scaleMesuredValue(valueMv, dfMv, resultScale);
        }
        else {
            Logger::getLogger()->debug("%s check validity failed", beforeLog.c_str());
            valueMv = 0;
        }

        this->createDetailQuality(dpQ, resultScale, checkValidity);

        DatapointValue dv(valueMv);
        auto dpmagF = new Datapoint(ConstantsMvScale::KeyMessagePivotJsonMagF, dv);

        Datapoint *dpmag = createDictElement(dpTyp, ConstantsMvScale::KeyMessagePivotJsonMag);
        DatapointValue *v = &(dpmag->getData());
        v->getDpVec()->push_back(dpmagF);            

        if (dfMv.getTypeScale() != ScaleType::TRANSPARENT && checkValidity) {
            createStringElement(dpQ, ConstantsMvScale::KeyMessagePivotJsonSource, ConstantsMvScale::ValueSubstituted);
        }

        Logger::getLogger()->debug("%s Value mesured %s, new value %f", beforeLog.c_str(), id.c_str(), valueMv);
    }

    (*m_func)(m_data, readingSet);    
}

/**
 * Reconfiguration entry point to the filter.
 *
 * This method runs holding the configMutex to prevent
 * ingest using the regex class that may be destroyed by this
 * call.
 *
 * Pass the configuration to the base FilterPlugin class and
 * then call the private method to handle the filter specific
 * configuration.
 *
 * @param newConfig  The JSON of the new configuration
 */
void FilterMvScale::reconfigure(const std::string& newConfig) {
    lock_guard<mutex> guard(m_configMutex);
    setConfig(newConfig);

    ConfigCategory config("newConfig", newConfig);
    if (config.itemExists("exchanged_data")) {
        this->setJsonConfig(config.getValue("exchanged_data"));
    }
}

/**
 * Creation of quality details according to the result of the quality validation
 * 
 * @param dpQ : Vector of datapoint for dicionary Q
 * @param resultScale : Scaling result status
 * @param checkValidity : Result of the quality control
*/
void FilterMvScale::createDetailQuality(Datapoints *dpQ, ScaleResult resultScale, bool checkValidity) {

    string beforeLog = ConstantsMvScale::NamePlugin + " - createDetailQuality : ";
    
    if (checkValidity) {

        if (resultScale != ScaleResult::NO_ERROR) {
            Datapoints *dpDetailQuality = findDictElement(dpQ, ConstantsMvScale::KeyMessagePivotJsonDetailQuality);
            if (dpDetailQuality == nullptr) {
                Datapoint *dp = createDictElement(dpQ, ConstantsMvScale::KeyMessagePivotJsonDetailQuality);
                dpDetailQuality = dp->getData().getDpVec();
            }
        
            if (resultScale == ScaleResult::INACURATE_VALUE) {
                Logger::getLogger()->debug("%s add inaccurate quality", beforeLog.c_str());
                createIntegerElement(dpDetailQuality, ConstantsMvScale::KeyMessagePivotJsonInaccurate, 1);
                createStringElement(dpQ, ConstantsMvScale::KeyMessagePivotJsonValidity, ConstantsMvScale::ValueQuestionable);
            }
            else if (resultScale == ScaleResult::OVERFLOW_VALUE) {
                Logger::getLogger()->debug("%s add overflow quality", beforeLog.c_str());
                createIntegerElement(dpDetailQuality, ConstantsMvScale::KeyMessagePivotJsonOverflow, 1);
                createStringElement(dpQ, ConstantsMvScale::KeyMessagePivotJsonValidity, ConstantsMvScale::ValueInvalid);
            }
            else if (resultScale == ScaleResult::INCONSISTENT_VALUE) {
                Logger::getLogger()->debug("%s add inconsistent quality", beforeLog.c_str());
                createIntegerElement(dpDetailQuality, ConstantsMvScale::KeyMessagePivotJsonInconsistent, 1);
                createStringElement(dpQ, ConstantsMvScale::KeyMessagePivotJsonValidity, ConstantsMvScale::ValueQuestionable);
            }
        }
    }
    else {
        Logger::getLogger()->debug("%s add invalid (check validity failed)", beforeLog.c_str());
        createStringElement(dpQ, ConstantsMvScale::KeyMessagePivotJsonValidity, "invalid");
    }
}

/**
 * Method for quality control
 * The quality is considered bad if PIVOTTM.GTIM.MvTyp.q.Validity is different of "good" or if one of the attributes of PIVOTTM.MvTyp.q.DetailQuality is set to "true" 
 * 
 * @param dpQ : Vector of datapoint for dicionary Q
 * @return true if quality is good, or else false
*/
bool FilterMvScale::checkValidity(Datapoints *dpQ) {
    
    bool validity = true;
    
    string strValidity = findStringElement(dpQ, ConstantsMvScale::KeyMessagePivotJsonValidity);
    if (strValidity.compare("good") != 0) {
        validity = false;
    }

    Datapoints *dpDetailQuality = findDictElement(dpQ, ConstantsMvScale::KeyMessagePivotJsonDetailQuality);
    if (dpDetailQuality != nullptr) {
        for (Datapoint *dp : *dpDetailQuality) {
            DatapointValue& data = dp->getData();
            const DatapointValue::dataTagType dType(data.getType());
            if (dType == DatapointValue::T_INTEGER) {
                if (data.toInt() == 1) {
                    validity = false;
                    break;
                }
            }
        }
    }
    return validity;
}