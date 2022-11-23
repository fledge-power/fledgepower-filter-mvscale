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

typedef enum {
        NORMAL,
        SQUARE_ROOT,
        QUADRATIC,
        TRANSPARENT
    } ScaleType;

typedef struct {
    float factorA;
    float factorB;
    float deadbandMin;
    float deadbandMax;
    ScaleType typeScale;
} DataExchangeDefinition;

class ConfigMvTyp {
public:  
    ConfigMvTyp();
    ~ConfigMvTyp();

    void importExchangedData(const std::string & exchangeConfig);
    void deleteExchangeDefinitions();

    std::map<std::string, DataExchangeDefinition*> & getExchangeDefinition() { return m_exchangeDefinitions; };

    DataExchangeDefinition * getDataExchangeWithID(std::string);

private:
    ScaleType getTypeScale(std::string tfid);
    std::map<std::string, DataExchangeDefinition*> m_exchangeDefinitions = std::map<std::string, DataExchangeDefinition*>();
};

#endif  // INCLUDE_CONFIG_MV_TYP_H_