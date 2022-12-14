#ifndef INCLUDE_SCALE_MV_H_
#define INCLUDE_SCALE_MV_H_

#include <configMvTyp.h>

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

enum class ScaleResult {
    NO_ERROR,
    INCONSISTENT_VALUE,
    OVERFLOW_VALUE,
    INACURATE_VALUE
};

class ScaleMV {
public:  
    ScaleMV() = default;
    
    float scaleMesuredValue(double value, DataExchangeDefinition defMv, ScaleResult& resultScale);
};

#endif  // INCLUDE_SCALE_MV_H_