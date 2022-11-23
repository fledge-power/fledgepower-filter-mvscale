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
#include <scaleMV.h>
#include <constants.h>
#include <datapointUtility.h>


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
    m_config = new ConfigMvTyp();
}

/**
 * Destructor for this filter class
 */
FilterMvScale::~FilterMvScale() {
    delete m_config;
}

/**
 * Modification of configuration
 * 
 * @param jsonExchanged : configuration ExchangedData
*/
void FilterMvScale::setJsonConfig(string jsonExchanged) {
    m_config->importExchangedData(jsonExchanged);
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
    if (isEnabled()) {
        // Just get all the readings in the readingset
        const std::vector<Reading*> & readings = readingSet->getAllReadings();
        for (auto reading = readings.cbegin(); reading != readings.cend(); reading++) {
            
            // Get datapoints on readings
            Datapoints & dataPoints = (*reading)->getReadingData();
            string assetName = (*reading)->getAssetName();

            Datapoints * dpPivotTM = findDictElement(&dataPoints, Constants::KEY_MESSAGE_PIVOT_JSON_ROOT);
            if (dpPivotTM == nullptr) {
                continue;
            }

            Datapoints * dpGtim = findDictElement(dpPivotTM, Constants::KEY_MESSAGE_PIVOT_JSON_GT);
            if (dpGtim == nullptr) {
                continue;
            }

            Datapoints * dpTyp = findDictElement(dpGtim, Constants::KEY_MESSAGE_PIVOT_JSON_CDC_MV);
            if (dpTyp == nullptr) {
                continue;
            }

            Datapoints * dpMag = findDictElement(dpTyp, Constants::KEY_MESSAGE_PIVOT_JSON_MAG);
            if (dpMag == nullptr) {
                continue;
            }

            string id = findStringElement(dpGtim, Constants::KEY_MESSAGE_PIVOT_JSON_ID);
            if (id.compare("") == 0) {
                continue;
            }

            double valueMv = 0;
            DatapointValue * dpValueMesure = findValueElement(dpMag, Constants::KEY_MESSAGE_PIVOT_JSON_MAG_I);
            if (dpValueMesure == nullptr) {
                dpValueMesure = findValueElement(dpMag, Constants::KEY_MESSAGE_PIVOT_JSON_MAG_F);
                if (dpValueMesure == nullptr) {
                    continue;
                }
            }

            if (dpValueMesure->getType() == DatapointValue::T_INTEGER) {
                valueMv = dpValueMesure->toInt();
            }
            else if (dpValueMesure->getType() == DatapointValue::T_FLOAT) {
                valueMv = dpValueMesure->toDouble();
            }
            else {
                continue;
            }

            DataExchangeDefinition * dfMv = m_config->getDataExchangeWithID(id);
            if (dfMv == nullptr) {
                continue;
            }

            Datapoints * dpQ = findDictElement(dpTyp, Constants::KEY_MESSAGE_PIVOT_JSON_Q);
            if (dpQ == nullptr) {
                continue;
            }

            ScaleResult resultScale;
            ScaleMV sc;           
            
            // Check quality
            bool checkValidity = this->checkValidity(dpQ);
            if(checkValidity) {
                valueMv = sc.scaleMesuredValue(valueMv, *dfMv, resultScale);
            }
            else {
                valueMv = 0;
            }

            this->createDetailQuality(dpQ, resultScale, checkValidity);

            DatapointValue dv(valueMv);
            Datapoint * dpmagF = new Datapoint(Constants::KEY_MESSAGE_PIVOT_JSON_MAG_F, dv);

            Datapoint * dpmag = createDictElement(dpTyp, Constants::KEY_MESSAGE_PIVOT_JSON_MAG);
            DatapointValue * v = &(dpmag->getData());
            v->getDpVec()->push_back(dpmagF);            

            if (dfMv->typeScale != ScaleType::TRANSPARENT && checkValidity) {
                createStringElement(dpQ, Constants::KEY_MESSAGE_PIVOT_JSON_SOURCE, Constants::VALUE_JSON_SUBSTITUTED);
            }

            Logger::getLogger()->debug("Value mesured %s, new value %f", id.c_str(), valueMv);
        }
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
void FilterMvScale::createDetailQuality(Datapoints * dpQ, ScaleResult resultScale, bool checkValidity) {

    if (checkValidity) {

        if (resultScale != ScaleResult::NO_ERROR) {
            Datapoints * dpDetailQuality = findDictElement(dpQ, Constants::KEY_MESSAGE_PIVOT_JSON_DETAIL_QUALITY);
            if (dpDetailQuality == nullptr) {
                Datapoint * dp = createDictElement(dpQ, Constants::KEY_MESSAGE_PIVOT_JSON_DETAIL_QUALITY);
                dpDetailQuality = dp->getData().getDpVec();
            }
        
            if (resultScale == ScaleResult::INACURATE_VALUE) {
                createIntegerElement(dpDetailQuality, Constants::KEY_MESSAGE_PIVOT_JSON_INACCURATE, 1);
                createStringElement(dpQ, Constants::KEY_MESSAGE_PIVOT_JSON_VALIDITY, Constants::VALUE_JSON_QUESTIONABLE);
            }
            else if (resultScale == ScaleResult::OVERFLOW_VALUE) {
                createIntegerElement(dpDetailQuality, Constants::KEY_MESSAGE_PIVOT_JSON_OVERFLOW, 1);
                createStringElement(dpQ, Constants::KEY_MESSAGE_PIVOT_JSON_VALIDITY, Constants::VALUE_JSON_INVALID);
            }
            else if (resultScale == ScaleResult::INCONSISTENT_VALUE) {
                createIntegerElement(dpDetailQuality, Constants::KEY_MESSAGE_PIVOT_JSON_INCONSISTENT, 1);
                createStringElement(dpQ, Constants::KEY_MESSAGE_PIVOT_JSON_VALIDITY, Constants::VALUE_JSON_QUESTIONABLE);
            }
        }
    }
    else {
        createStringElement(dpQ, Constants::KEY_MESSAGE_PIVOT_JSON_VALIDITY, "invalid");
    }
}

/**
 * Method for quality control
 * The quality is considered bad if PIVOTTM.GTIM.MvTyp.q.Validity is different of "good" or if one of the attributes of PIVOTTM.MvTyp.q.DetailQuality is set to "true" 
 * 
 * @param dpQ : Vector of datapoint for dicionary Q
 * @return true if quality is good, or else false
*/
bool FilterMvScale::checkValidity(Datapoints * dpQ) {
    
    bool validity = true;
    
    string strValidity = findStringElement(dpQ, Constants::KEY_MESSAGE_PIVOT_JSON_VALIDITY);
    if (strValidity.compare("good") != 0) {
        validity = false;
    }

    Datapoints * dpDetailQuality = findDictElement(dpQ, Constants::KEY_MESSAGE_PIVOT_JSON_DETAIL_QUALITY);
    if (dpDetailQuality != nullptr) {
        for (Datapoint* dp : * dpDetailQuality) {
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