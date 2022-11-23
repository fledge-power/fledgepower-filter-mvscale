#ifndef INCLUDE_CONSTANTS_H_
#define INCLUDE_CONSTANTS_H_

#include <string>

struct Constants {
    static const char * JSON_EXCHANGED_DATA;
    static const char * JSON_DATAPOINTS;
    static const char * JSON_PIVOT_ID;
    static const char * JSON_TFID;
    static const char * JSON_DEADBAND;
    static const char * JSON_PARAMS;
    static const char * JSON_PIVOT_TYPE;

    static const std::string JSON_ATTR_MVTYP;
    static const std::string JSON_ATTR_LAW_NORMAL;
    static const std::string JSON_ATTR_LAW_QUADRATIC;
    static const std::string JSON_ATTR_LAW_SQUARE_ROOT;

    static const std::string KEY_MESSAGE_PIVOT_JSON_ROOT;
    static const std::string KEY_MESSAGE_PIVOT_JSON_GT;
    static const std::string KEY_MESSAGE_PIVOT_JSON_CDC_MV;
    static const std::string KEY_MESSAGE_PIVOT_JSON_MAG;
    static const std::string KEY_MESSAGE_PIVOT_JSON_MAG_I;
    static const std::string KEY_MESSAGE_PIVOT_JSON_MAG_F;
    static const std::string KEY_MESSAGE_PIVOT_JSON_ID;
    static const std::string KEY_MESSAGE_PIVOT_JSON_Q;
    static const std::string KEY_MESSAGE_PIVOT_JSON_DETAIL_QUALITY;
    static const std::string KEY_MESSAGE_PIVOT_JSON_INCONSISTENT;
    static const std::string KEY_MESSAGE_PIVOT_JSON_OVERFLOW;
    static const std::string KEY_MESSAGE_PIVOT_JSON_INACCURATE;
    static const std::string KEY_MESSAGE_PIVOT_JSON_VALIDITY;
    static const std::string KEY_MESSAGE_PIVOT_JSON_SOURCE;

    static const std::string VALUE_JSON_SUBSTITUTED;
    static const std::string VALUE_JSON_INVALID;
    static const std::string VALUE_JSON_QUESTIONABLE;
};

#endif //INCLUDE_CONSTANTS_H_