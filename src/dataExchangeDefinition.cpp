#include <dataExchangeDefinition.h>
#include <constantsMvScale.h>

/**
 * Constructor
*/
DataExchangeDefinition::DataExchangeDefinition():
    m_factorA(-1),
    m_factorB(-1),
    m_deadbandMin(-1),
    m_deadbandMax(-1),
    m_typeScale(ScaleType::NORMAL)
{}

/**
 * Operator of recopy
*/
bool operator==(const DataExchangeDefinition& lhs, const DataExchangeDefinition& rhs)
{
    return lhs.getFactorA() == rhs.getFactorA() && lhs.getFactorB() == rhs.getFactorB()
        && lhs.getDeadBandMin() == rhs.getDeadBandMin() && lhs.getDeadBandMax() == rhs.getDeadBandMax()
        && lhs.getTypeScale() == rhs.getTypeScale();
}
