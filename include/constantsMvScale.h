#ifndef INCLUDE_CONSTANTS_H_
#define INCLUDE_CONSTANTS_H_

#include <string>

#define FILTER_NAME "mvscale"

struct ConstantsMvScale {

    static const std::string NamePlugin;
    
    static const char *JsonExchangedData;
    static const char *JsonDatapoints;
    static const char *JsonPivotId;
    static const char *JsonTfid;
    static const char *JsonDeadband;
    static const char *JsonParams;
    static const char *JsonPivotType;

    static const std::string JsonAttrMvtyp;
    static const std::string JsonAttrLawNormal;
    static const std::string JsonAttrLawQuadratic;
    static const std::string JsonAttrLawSquareRoot;

    static const std::string KeyMessagePivotJsonRoot;
    static const std::string KeyMessagePivotJsonGt;
    static const std::string KeyMessagePivotJsonCdcMv;
    static const std::string KeyMessagePivotJsonMag;
    static const std::string KeyMessagePivotJsonMagI;
    static const std::string KeyMessagePivotJsonMagF;
    static const std::string KeyMessagePivotJsonId;
    static const std::string KeyMessagePivotJsonQ;
    static const std::string KeyMessagePivotJsonDetailQuality;
    static const std::string KeyMessagePivotJsonInconsistent;
    static const std::string KeyMessagePivotJsonOverflow;
    static const std::string KeyMessagePivotJsonInaccurate;
    static const std::string KeyMessagePivotJsonValidity;
    static const std::string KeyMessagePivotJsonSource;

    static const std::string ValueSubstituted;
    static const std::string ValueInvalid;
    static const std::string ValueQuestionable;
};

#endif //INCLUDE_CONSTANTS_H_