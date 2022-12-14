#ifndef INCLUDE_DATA_EXCHANGE_DEFINITION_H_
#define INCLUDE_DATA_EXCHANGE_DEFINITION_H_

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

enum class ScaleType {
    NORMAL,
    SQUARE_ROOT,
    QUADRATIC,
    TRANSPARENT
};

class DataExchangeDefinition { 

public:
    DataExchangeDefinition();

    constexpr float getFactorA() const { return m_factorA; }
    constexpr float getFactorB() const { return m_factorB; }
    constexpr float getDeadBandMin() const { return m_deadbandMin; }
    constexpr float getDeadBandMax() const { return m_deadbandMax; }
    constexpr ScaleType getTypeScale() const { return m_typeScale; }

    void setFactorA(float fA) { m_factorA = fA; }
    void setFactorB(float fB) { m_factorB = fB; }
    void setDeadBandMin(float dMin) { m_deadbandMin = dMin; }
    void setDeadBandMax(float dMax) { m_deadbandMax = dMax; }
    void setTypeScale(ScaleType s) { m_typeScale = s; }

private:
    float m_factorA;
    float m_factorB;
    float m_deadbandMin;
    float m_deadbandMax;
    ScaleType m_typeScale;
};

bool operator==(const DataExchangeDefinition& lhs, const DataExchangeDefinition& rhs);

#endif  // INCLUDE_DATA_EXCHANGE_DEFINITION_H_