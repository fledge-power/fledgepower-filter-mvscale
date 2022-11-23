#ifndef INCLUDE_FILTER_MV_SCALE_H_
#define INCLUDE_FILTER_MV_SCALE_H_

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
#include <configMvTyp.h>
#include <datapointUtility.h>
#include <scaleMV.h>

#include <config_category.h>
#include <filter.h>
#include <mutex>
#include <string>

class FilterMvScale  : public FledgeFilter
{
public:  
    FilterMvScale(const std::string& filterName,
                        ConfigCategory& filterConfig,
                        OUTPUT_HANDLE *outHandle,
                        OUTPUT_STREAM output);

    ~FilterMvScale();

    void ingest(READINGSET *readingSet);
    void reconfigure(const std::string& newConfig);

    void setJsonConfig(std::string jsonExchanged);

    ConfigMvTyp * getConfig() { return m_config; }

private:

    void createDetailQuality(DatapointUtility::Datapoints * dpTyp, ScaleResult resultScale, bool checkQuality);
    bool checkValidity(DatapointUtility::Datapoints * dpQ);

    std::mutex      m_configMutex;
    ConfigMvTyp*    m_config;
};

#endif  // INCLUDE_FILTER_MV_SCALE_H_