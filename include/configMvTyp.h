#ifndef INCLUDE_CONFIG_MV_TYP_H_
#define INCLUDE_CONFIG_MV_TYP_H_

/*
 * Import confiugration Exchanged data
 *
 * Copyright (c) 2020, RTE (https://www.rte-france.com)
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Yannick Marchetaux
 * 
 */
#include <map>
#include <string>
#include <dataExchangeDefinition.h>

class ConfigMvTyp {
public:  
    ConfigMvTyp();

    void importExchangedData(const std::string& exchangeConfig);
    DataExchangeDefinition getDataExchangeWithID(const std::string& id);
    
    std::map<std::string, DataExchangeDefinition>& getExchangeDefinition() { return m_exchangeDefinitions; };

private:
    ScaleType getTypeScale(const std::string& tfid);
    std::map<std::string, DataExchangeDefinition> m_exchangeDefinitions;
};

#endif  // INCLUDE_CONFIG_MV_TYP_H_