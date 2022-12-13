#include <scaleMV.h>
#include <cmath>
#include <limits>

using namespace std;

/**
 * Method for scaling the measured value
 * 
 * @param valueMesured : mesured value
 * @param defMv : configuration of the measured value
 * @param resultScale : return of the scaling status
 * @return the calculated value
*/
float ScaleMV::scaleMesuredValue(double valueMesured, DataExchangeDefinition defMv, ScaleResult& resultScale) {

    double value = 0;
    resultScale = ScaleResult::NO_ERROR;

    // Verify inacurate value (deadband)
    if (valueMesured > defMv.getDeadBandMin() && valueMesured < defMv.getDeadBandMax()) {
        resultScale = ScaleResult::INACURATE_VALUE;
        return (float)valueMesured;
    }

    // Scale of value mesured
    switch (defMv.getTypeScale()) {
        case ScaleType::NORMAL:
            value = defMv.getFactorA() * valueMesured + defMv.getFactorB();
            break; 
        case ScaleType::SQUARE_ROOT:
            if (valueMesured > 0) {
                value = defMv.getFactorA() * sqrt(valueMesured) + defMv.getFactorB();
            }
            else {
                value = 0;
                resultScale = ScaleResult::INCONSISTENT_VALUE;
            }
            break; 
        case ScaleType::QUADRATIC:
            if ((defMv.getFactorA() * valueMesured + defMv.getFactorB()) > 0) {
                value = sqrt(defMv.getFactorA() * valueMesured + defMv.getFactorB());
            }
            else {
                value = 0;
                resultScale = ScaleResult::INCONSISTENT_VALUE;
            }
            break; 
        default:
            value = valueMesured;
            break;
    }

    // Check for overflow
    if (value < std::numeric_limits<float>::lowest() || value > std::numeric_limits<float>::max()) {
        resultScale = ScaleResult::OVERFLOW_VALUE;
        value = 0;
    }   

    return (float)value;
}