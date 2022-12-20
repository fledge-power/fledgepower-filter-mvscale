#ifndef INCLUDE_CONSTANTS_MV_SCALE_H_
#define INCLUDE_CONSTANTS_MV_SCALE_H_

#include <string>

#define FILTER_NAME "mvscale"

namespace ConstantsMvScale {

    static const std::string NamePlugin             = FILTER_NAME;
    
    static const char *JsonExchangedData            = "exchanged_data";
    static const char *JsonDatapoints               = "datapoints";
    static const char *JsonPivotType                = "pivot_type";
    static const char *JsonPivotId                  = "pivot_id";
    static const char *JsonTfid                     = "tfid";
    static const char *JsonDeadband                 = "deadband";
    static const char *JsonParams                   = "params";

    static const std::string JsonAttrMvtyp          = "MvTyp";
    static const std::string JsonAttrLawNormal      = "normal";
    static const std::string JsonAttrLawQuadratic   = "quadratic";
    static const std::string JsonAttrLawSquareRoot  = "square_root";

    static const std::string KeyMessagePivotJsonRoot    = "PIVOT";
    static const std::string KeyMessagePivotJsonGt      = "GTIM";
    static const std::string KeyMessagePivotJsonCdcMv   = "MvTyp";
    static const std::string KeyMessagePivotJsonMag     = "mag";
    static const std::string KeyMessagePivotJsonMagI    = "i";
    static const std::string KeyMessagePivotJsonMagF    = "f";
    static const std::string KeyMessagePivotJsonId      = "Identifier";
    static const std::string KeyMessagePivotJsonQ       = "q";
    static const std::string KeyMessagePivotJsonDetailQuality   = "DetailQuality";
    static const std::string KeyMessagePivotJsonInconsistent    = "inconsistent";
    static const std::string KeyMessagePivotJsonOverflow        = "overflow";
    static const std::string KeyMessagePivotJsonInaccurate      = "inaccurate";
    static const std::string KeyMessagePivotJsonValidity        = "Validity";
    static const std::string KeyMessagePivotJsonSource          = "Source";

    static const std::string ValueSubstituted                 = "substituted";
    static const std::string ValueInvalid                     = "invalid";
    static const std::string ValueQuestionable                = "questionable";
};

#endif //INCLUDE_CONSTANTS_MV_SCALE_H_