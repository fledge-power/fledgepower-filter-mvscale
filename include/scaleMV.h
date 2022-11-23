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

typedef enum {
    NO_ERROR,
    INCONSISTENT_VALUE,
    OVERFLOW_VALUE,
    INACURATE_VALUE
} ScaleResult;

class ScaleMV {
public:  
    ScaleMV();
    ~ScaleMV();

    float scaleMesuredValue(double value, DataExchangeDefinition defMv, ScaleResult & resultScale);
};

#endif  // INCLUDE_SCALE_MV_H_