#include "rapidjson/document.h"

#include <configMvTyp.h>
#include <constantsMvScale.h>

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
        DataExchangeDefinition *dp = exchangeDefintions.second;
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
void ConfigMvTyp::importExchangedData(const string& exchangeConfig) {
    
    deleteExchangeDefinitions();
    Document document;

    if (document.Parse(const_cast<char*>(exchangeConfig.c_str())).HasParseError()) {
        Logger::getLogger()->fatal("Parsing error in data exchange configuration");
        printf("Parsing error in data exchange configuration\n");
        return;
    }

    if (!document.IsObject())
        return;

    if (!document.HasMember(ConstantsMvScale::JsonExchangedData) || !document[ConstantsMvScale::JsonExchangedData].IsObject()) {
        return;
    }
    const Value& exchangeData = document[ConstantsMvScale::JsonExchangedData];

    if (!exchangeData.HasMember(ConstantsMvScale::JsonDatapoints) || !exchangeData[ConstantsMvScale::JsonDatapoints].IsArray()) {
        return;
    }
    const Value& datapoints = exchangeData[ConstantsMvScale::JsonDatapoints];

    for (const Value& datapoint : datapoints.GetArray()) {
        
        if (!datapoint.IsObject()) continue;
        
        if (!datapoint.HasMember(ConstantsMvScale::JsonPivotType) || !datapoint[ConstantsMvScale::JsonPivotType].IsString()) {
            continue;
        }
        string type = datapoint[ConstantsMvScale::JsonPivotType].GetString();
        if (type != ConstantsMvScale::JsonAttrMvtyp) {
            continue;
        }

        if (!datapoint.HasMember(ConstantsMvScale::JsonPivotId) || !datapoint[ConstantsMvScale::JsonPivotId].IsString()) {
            continue;
        }
        string pivot_id = datapoint[ConstantsMvScale::JsonPivotId].GetString();

        if (!datapoint.HasMember(ConstantsMvScale::JsonTfid) || !datapoint[ConstantsMvScale::JsonTfid].IsString()) {
            continue;
        }
        string tfid = datapoint[ConstantsMvScale::JsonTfid].GetString();

        if (!datapoint.HasMember(ConstantsMvScale::JsonParams) || !datapoint[ConstantsMvScale::JsonParams].IsArray()) {
            continue;
        }
        auto params = datapoint[ConstantsMvScale::JsonParams].GetArray();

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

        if (!datapoint.HasMember(ConstantsMvScale::JsonDeadband) || !datapoint[ConstantsMvScale::JsonDeadband].IsArray()) {
            continue;
        }
        auto deadband = datapoint[ConstantsMvScale::JsonDeadband].GetArray();

        float deadbandMax = 0;
        float deadbandMin = 0;

        if (deadband.Size() >= 2) {
            deadbandMax = deadband[1].GetFloat();
            deadbandMin = deadband[0].GetFloat();
        }
        else {
            continue;
        }

        DataExchangeDefinition *config = new DataExchangeDefinition;
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
    if (tfid == ConstantsMvScale::JsonAttrLawNormal) {
        return ScaleType::NORMAL;
    }
    else if (tfid == ConstantsMvScale::JsonAttrLawSquareRoot) {
        return ScaleType::SQUARE_ROOT;
    }
    else if (tfid == ConstantsMvScale::JsonAttrLawQuadratic) {
        return ScaleType::QUADRATIC;
    }
    return ScaleType::TRANSPARENT;
}

/**
 * Get the configuration data by searching with its identifier
 * 
 * @param id : identifier
 * @return the confiugation of a value measured
*/
DataExchangeDefinition *ConfigMvTyp::getDataExchangeWithID(std::string id) {
    if(m_exchangeDefinitions.find(id) != m_exchangeDefinitions.end()) {
        return m_exchangeDefinitions[id];
    }
    return nullptr;
}