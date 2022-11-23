#include "rapidjson/document.h"

#include <configMvTyp.h>
#include <constants.h>

#include <logger.h>
#include <cctype>

using namespace std;
using namespace rapidjson;

/**
 * Constructor
*/
ConfigMvTyp::ConfigMvTyp() {
    m_exchangeDefinitions.clear();
}

/**
 * Destructor
*/
ConfigMvTyp::~ConfigMvTyp() {
    deleteExchangeDefinitions();
}

/**
 * Deletion of loaded data
*/
void ConfigMvTyp::deleteExchangeDefinitions() {
    for (auto const& exchangeDefintions : m_exchangeDefinitions) {
        DataExchangeDefinition* dp = exchangeDefintions.second;
        delete dp;
    }

    m_exchangeDefinitions.clear();
}

/**
 * Import data in the form of Exchanged_data
 * The data is saved in a map m_exchangeDefinitions
 * 
 * @param exchangeConfig : configuration Exchanged_data as a string 
*/
void ConfigMvTyp::importExchangedData(const string & exchangeConfig) {
    
    deleteExchangeDefinitions();
    Document document;

    if (document.Parse(const_cast<char*>(exchangeConfig.c_str())).HasParseError()) {
        Logger::getLogger()->fatal("Parsing error in data exchange configuration");
        printf("Parsing error in data exchange configuration\n");
        return;
    }

    if (!document.IsObject())
        return;

    if (!document.HasMember(Constants::JSON_EXCHANGED_DATA) || !document[Constants::JSON_EXCHANGED_DATA].IsObject()) {
        return;
    }
    const Value& exchangeData = document[Constants::JSON_EXCHANGED_DATA];

    if (!exchangeData.HasMember(Constants::JSON_DATAPOINTS) || !exchangeData[Constants::JSON_DATAPOINTS].IsArray()) {
        return;
    }
    const Value& datapoints = exchangeData[Constants::JSON_DATAPOINTS];

    for (const Value& datapoint : datapoints.GetArray()) {
        
        if (!datapoint.IsObject()) continue;
        
        if (!datapoint.HasMember(Constants::JSON_PIVOT_TYPE) || !datapoint[Constants::JSON_PIVOT_TYPE].IsString()) {
            continue;
        }
        string type = datapoint[Constants::JSON_PIVOT_TYPE].GetString();
        if (type != Constants::JSON_ATTR_MVTYP) {
            continue;
        }

        if (!datapoint.HasMember(Constants::JSON_PIVOT_ID) || !datapoint[Constants::JSON_PIVOT_ID].IsString()) {
            continue;
        }
        string pivot_id = datapoint[Constants::JSON_PIVOT_ID].GetString();

        if (!datapoint.HasMember(Constants::JSON_TFID) || !datapoint[Constants::JSON_TFID].IsString()) {
            continue;
        }
        string tfid = datapoint[Constants::JSON_TFID].GetString();

        if (!datapoint.HasMember(Constants::JSON_PARAMS) || !datapoint[Constants::JSON_PARAMS].IsArray()) {
            continue;
        }
        auto params = datapoint[Constants::JSON_PARAMS].GetArray();

        float factorB = 0;
        float factorA = 0;
        if (params.Size() >= 2) {
            factorB = params[1].GetFloat();
            factorA = params[0].GetFloat();
        }
        else {
            continue;
        }

        if (factorA == 0) {
            factorA = 1;
        }

        if (!datapoint.HasMember(Constants::JSON_DEADBAND) || !datapoint[Constants::JSON_DEADBAND].IsArray()) {
            continue;
        }
        auto deadband = datapoint[Constants::JSON_DEADBAND].GetArray();

        float deadbandMax = 0;
        float deadbandMin = 0;

        if (deadband.Size() >= 2) {
            deadbandMax = deadband[1].GetFloat();
            deadbandMin = deadband[0].GetFloat();
        }
        else {
            continue;
        }

        DataExchangeDefinition* config = new DataExchangeDefinition;
        if (config) {
            config->deadbandMax = deadbandMax;
            config->deadbandMin = deadbandMin;
            config->factorA = factorA;
            config->factorB = factorB;
            config->typeScale = this->getTypeScale(tfid);
            m_exchangeDefinitions[pivot_id] = config;

            Logger::getLogger()->debug("Reconfiguration of %s, factorA[%f], factorB[%f], deadbandMin[%f], deadbandMax[%f], typeScale[%d/%s]",
                pivot_id.c_str(), config->factorA, config->factorB, config->deadbandMin, config->deadbandMax, config->typeScale, tfid.c_str());
        }   
    }
}

/**
 * Reading the scaling law
 * 
 * @param tfid : law in the form of a string
 * @return Law in the form of an enum
*/
ScaleType ConfigMvTyp::getTypeScale(string tfid) {
    if (tfid == Constants::JSON_ATTR_LAW_NORMAL) {
        return ScaleType::NORMAL;
    }
    else if (tfid == Constants::JSON_ATTR_LAW_SQUARE_ROOT) {
        return ScaleType::SQUARE_ROOT;
    }
    else if (tfid == Constants::JSON_ATTR_LAW_QUADRATIC) {
        return ScaleType::QUADRATIC;
    }
    return ScaleType::TRANSPARENT;
}

/**
 * 
*/
DataExchangeDefinition * ConfigMvTyp::getDataExchangeWithID(std::string id) {
    if(m_exchangeDefinitions.find(id) != m_exchangeDefinitions.end()) {
        return m_exchangeDefinitions[id];
    }
    return nullptr;
}