#include <scaleMV.h>
#include <cmath>
#include <limits>

using namespace std;

/**
 * Constructor
*/
ScaleMV::ScaleMV() {
}

/**
 * Destructor
*/
ScaleMV::~ScaleMV() {
}

/**
 * Method for scaling the measured value
 * 
 * @param valueMesured : mesured value
 * @param defMv : configuration of the measured value
 * @param resultScale : return of the scaling status
 * @return the calculated value
*/
float ScaleMV::scaleMesuredValue(double valueMesured, DataExchangeDefinition defMv, ScaleResult & resultScale) {

    double value = 0;
    resultScale = ScaleResult::NO_ERROR;

    // Verify inacurate value (deadband)
    if (valueMesured > defMv.deadbandMin && valueMesured < defMv.deadbandMax) {
        resultScale = ScaleResult::INACURATE_VALUE;
        return valueMesured;
    }

    // Scale of value mesured
    switch (defMv.typeScale) {
        case NORMAL:
            value = defMv.factorA * valueMesured + defMv.factorB;
            break; 
        case SQUARE_ROOT:
            if (valueMesured > 0) {
                value = defMv.factorA * sqrt(valueMesured) + defMv.factorB;
            }
            else {
                value = 0;
                resultScale = ScaleResult::INCONSISTENT_VALUE;
            }
            break; 
        case QUADRATIC:
            if ((defMv.factorA * valueMesured + defMv.factorB) > 0) {
                value = sqrt(defMv.factorA * valueMesured + defMv.factorB);
            }
            else {
                value = 0;
                resultScale = ScaleResult::INCONSISTENT_VALUE;
            }
            break; 
        case TRANSPARENT:
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