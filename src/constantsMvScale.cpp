#include <constantsMvScale.h>

const std::string ConstantsMvScale::NamePlugin             = FILTER_NAME;
    
const char *ConstantsMvScale::JsonExchangedData            = "exchanged_data";
const char *ConstantsMvScale::JsonDatapoints               = "datapoints";
const char *ConstantsMvScale::JsonPivotType                = "pivot_type";
const char *ConstantsMvScale::JsonPivotId                  = "pivot_id";
const char *ConstantsMvScale::JsonTfid                     = "tfid";
const char *ConstantsMvScale::JsonDeadband                 = "deadband";
const char *ConstantsMvScale::JsonParams                   = "params";

const std::string ConstantsMvScale::JsonAttrMvtyp          = "MvTyp";
const std::string ConstantsMvScale::JsonAttrLawNormal      = "normal";
const std::string ConstantsMvScale::JsonAttrLawQuadratic   = "quadratic";
const std::string ConstantsMvScale::JsonAttrLawSquareRoot  = "square_root";

const std::string ConstantsMvScale::KeyMessagePivotJsonRoot    = "PIVOTTM";
const std::string ConstantsMvScale::KeyMessagePivotJsonGt      = "GTIM";
const std::string ConstantsMvScale::KeyMessagePivotJsonCdcMv   = "MvTyp";
const std::string ConstantsMvScale::KeyMessagePivotJsonMag     = "mag";
const std::string ConstantsMvScale::KeyMessagePivotJsonMagI    = "i";
const std::string ConstantsMvScale::KeyMessagePivotJsonMagF    = "f";
const std::string ConstantsMvScale::KeyMessagePivotJsonId      = "Identifier";
const std::string ConstantsMvScale::KeyMessagePivotJsonQ       = "q";
const std::string ConstantsMvScale::KeyMessagePivotJsonDetailQuality   = "DetailQuality";
const std::string ConstantsMvScale::KeyMessagePivotJsonInconsistent    = "inconsistent";
const std::string ConstantsMvScale::KeyMessagePivotJsonOverflow        = "overflow";
const std::string ConstantsMvScale::KeyMessagePivotJsonInaccurate      = "inaccurate";
const std::string ConstantsMvScale::KeyMessagePivotJsonValidity        = "Validity";
const std::string ConstantsMvScale::KeyMessagePivotJsonSource          = "Source";

const std::string ConstantsMvScale::ValueSubstituted                 = "substituted";
const std::string ConstantsMvScale::ValueInvalid                     = "invalid";
const std::string ConstantsMvScale::ValueQuestionable                = "questionable";
