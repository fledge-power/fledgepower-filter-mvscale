#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filterMvScale.h>
#include <jsonToDatapoints.h>
#include <constantsMvScale.h>

using namespace std;
using namespace DatapointUtility;
using namespace JsonToDatapoints;

static string nameReading = "data_test";

static string configure = QUOTE({
    "enable" :{
        "value": "true"
    },
    "exchanged_data": {
        "value" : {
            "exchanged_data": {
                "datapoints" : [          
                    {
                        "label":"TM-1",
                        "pivot_id":"M_2367_3_15_4",
                        "pivot_type":"MvTyp",
                        "tfid": "normal",
                        "params": [1.0, 0.0],
                        "deadband" : [-10.0, 10.0],
                        "protocols":[
                            {
                                "name":"IEC104",
                                "typeid":"M_ME_NC_1",
                                "address":"3271612"
                            }
                        ]
                    },
                    {
                        "label":"TM-2",
                        "pivot_id":"M_2367_3_15_5",
                        "pivot_type":"MvTyp",
                        "tfid": "square_root",
                        "params": [1.0, 0.0],
                        "deadband" : [1.0, 1.0],
                        "protocols":[
                            {
                                "name":"IEC104",
                                "typeid":"M_ME_NC_1",
                                "address":"3271612"
                            }
                        ]
                    },
                    {
                        "label":"TM-3",
                        "pivot_id":"M_2367_3_15_6",
                        "pivot_type":"MvTyp",
                        "tfid": "quadratic",
                        "params": [1.0, 0.0],
                        "deadband" : [1.0, 1.0],
                        "protocols":[
                            {
                                "name":"IEC104",
                                "typeid":"M_ME_NC_1",
                                "address":"3271612"
                            }
                        ]
                    },
                    {
                        "label":"TM-4",
                        "pivot_id":"M_2367_3_15_7",
                        "pivot_type":"MvTyp",
                        "tfid": "normal",
                        "params": [100000000.0, 100000000.0],
                        "deadband" : [1.0, 1.0],
                        "protocols":[
                            {
                                "name":"IEC104",
                                "typeid":"M_ME_NC_1",
                                "address":"3271612"
                            }
                        ]
                    }
                ]
            }
        }
    }
});

static string jsonMessagePivotMvInDeadband = QUOTE({
	"PIVOTTM": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
            "Identifier": "M_2367_3_15_4",
            "MvTyp": {
                "mag": {
                    "i": 2
                },
                 "q": {
                    "Source": "process",
                    "Validity": "good"
                },
                "t": {
                    "SecondSinceEpoch": 1668759955
                }
            },            
            "TmOrg": {
                "stVal": "genuine"
            },
            "TmValidity": {
                "stVal": "VALID"
            }
        }
    }
});

static string jsonMessagePivotMvSquareRootNeg = QUOTE({
	"PIVOTTM": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
            "Identifier": "M_2367_3_15_5",
            "MvTyp": {
                "mag": {
                    "i": -2
                },
                 "q": {
                    "Source": "process",
                    "Validity": "good"
                },
                "t": {
                    "SecondSinceEpoch": 1668759955
                }
            },            
            "TmOrg": {
                "stVal": "genuine"
            },
            "TmValidity": {
                "stVal": "VALID"
            }
        }
    }
});

static string jsonMessagePivotMvQuadraNeg = QUOTE({
	"PIVOTTM": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
            "Identifier": "M_2367_3_15_6",
            "MvTyp": {
                "mag": {
                    "f": -2
                },
                 "q": {
                    "Source": "process",
                    "Validity": "good"
                },
                "t": {
                    "SecondSinceEpoch": 1668759955
                }
            },            
            "TmOrg": {
                "stVal": "genuine"
            },
            "TmValidity": {
                "stVal": "VALID"
            }
        }
    }
});

static string jsonMessagePivotMvOverflow = QUOTE({
	"PIVOTTM": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
            "Identifier": "M_2367_3_15_7",
            "MvTyp": {
                "mag": {
                    "f": 10000000000000000000000000000000.0
                },
                 "q": {
                    "Source": "process",
                    "Validity": "good"
                },
                "t": {
                    "SecondSinceEpoch": 1668759955
                }
            },            
            "TmOrg": {
                "stVal": "genuine"
            },
            "TmValidity": {
                "stVal": "VALID"
            }
        }
    }
});
 
extern "C" {
	PLUGIN_INFORMATION *plugin_info();
	void plugin_ingest(void *handle, READINGSET *readingSet);
	PLUGIN_HANDLE plugin_init(ConfigCategory *config,
			  OUTPUT_HANDLE *outHandle,
			  OUTPUT_STREAM output);
	
	void HandlerTestExceptionCalcul(void *handle, READINGSET *readings) {
		*(READINGSET **)handle = readings;
	}

    void plugin_reconfigure(PLUGIN_HANDLE *handle, const string& newConfig);
};

class TestExceptionCalcul : public testing::Test
{
protected:
    FilterMvScale *filter = nullptr;  // Object on which we call for tests
    ReadingSet *resultReading;

    // Setup is ran for every tests, so each variable are reinitialised
    void SetUp() override
    {
        PLUGIN_INFORMATION *info = plugin_info();
		ConfigCategory *config = new ConfigCategory("mvscale", info->config);
		
		ASSERT_NE(config, (ConfigCategory *)NULL);		
		config->setItemsValueFromDefault();
		config->setValue("enable", "true");
		
		void *handle = plugin_init(config, &resultReading, HandlerTestExceptionCalcul);
		filter = (FilterMvScale *) handle;

        plugin_reconfigure((PLUGIN_HANDLE*)filter, configure);
    }

    // TearDown is ran for every tests, so each variable are destroyed again
    void TearDown() override
    {
        delete filter;
    }

	void startTests(string json, ScaleResult resultCalcul, double valueResult, string validityExpected) {
		ASSERT_NE(filter, (void *)NULL);

        // Create Reading
        Datapoints *p = parseJson(json);

		Reading *reading = new Reading(nameReading, *p);

        Readings *readings = new Readings;
        readings->push_back(reading);

        // Create ReadingSet
        ReadingSet *readingSet = new ReadingSet(readings);
		
        plugin_ingest(filter, (READINGSET*)readingSet);
        Readings results = resultReading->getAllReadings();
        ASSERT_EQ(results.size(), 1);

        Reading *out = results[0];
        ASSERT_STREQ(out->getAssetName().c_str(), nameReading.c_str());
        ASSERT_EQ(out->getDatapointCount(), 1);

        Datapoints points = out->getReadingData();
        ASSERT_EQ(points.size(), 1);

        verifyDatapoint(&points, resultCalcul, valueResult, validityExpected);
        delete reading;
	}

	void verifyDatapoint(Datapoints *dps, ScaleResult resultCalcul, double valueResult, string validityExpected) {
		Datapoints *dpPivot = findDictElement(dps, ConstantsMvScale::KeyMessagePivotJsonRoot);
		ASSERT_NE(dpPivot, nullptr);
		
		Datapoints *dpGi = findDictElement(dpPivot, ConstantsMvScale::KeyMessagePivotJsonGt);
		ASSERT_NE(dpGi, nullptr);
		
		Datapoints *dpTyp = findDictElement(dpGi, ConstantsMvScale::KeyMessagePivotJsonCdcMv);
		ASSERT_NE(dpTyp, nullptr);
		
        Datapoints *dpMag = findDictElement(dpTyp, ConstantsMvScale::KeyMessagePivotJsonMag);
        ASSERT_NE(dpMag, nullptr);

        DatapointValue *vI = findValueElement(dpMag, ConstantsMvScale::KeyMessagePivotJsonMagI);
        ASSERT_EQ(vI, nullptr);

        DatapointValue *valueMv = findValueElement(dpMag, ConstantsMvScale::KeyMessagePivotJsonMagF);
        ASSERT_NE(valueMv, nullptr);

        ASSERT_FLOAT_EQ(valueMv->toDouble(), valueResult);

        Datapoints *dpQ = findDictElement(dpTyp, ConstantsMvScale::KeyMessagePivotJsonQ);
        ASSERT_NE(dpQ, nullptr);

        Datapoints *dpDetailQuality = findDictElement(dpQ, ConstantsMvScale::KeyMessagePivotJsonDetailQuality);
        ASSERT_NE(dpDetailQuality, nullptr);

        DatapointValue *valueDetailQuality = nullptr;

        switch(resultCalcul) {
            case ScaleResult::INACURATE_VALUE : 
                valueDetailQuality = findValueElement(dpDetailQuality, ConstantsMvScale::KeyMessagePivotJsonInaccurate);
                break;
            case ScaleResult::INCONSISTENT_VALUE : 
                valueDetailQuality = findValueElement(dpDetailQuality, ConstantsMvScale::KeyMessagePivotJsonInconsistent);
                break;
            case ScaleResult::OVERFLOW_VALUE : 
            default:
                valueDetailQuality = findValueElement(dpDetailQuality, ConstantsMvScale::KeyMessagePivotJsonOverflow);
                break;
        }
        ASSERT_EQ(valueDetailQuality->toInt(), 1);

        DatapointValue *vSource = findValueElement(dpQ, ConstantsMvScale::KeyMessagePivotJsonSource);
		ASSERT_NE(vSource, nullptr);

        ASSERT_STREQ(vSource->toStringValue().c_str(), "substituted"); 

        string validity = findStringElement(dpQ, ConstantsMvScale::KeyMessagePivotJsonValidity);
        ASSERT_STREQ(validity.c_str(), validityExpected.c_str());
	}
};

TEST_F(TestExceptionCalcul, MessagePivotMvInDeadband) 
{
	startTests(jsonMessagePivotMvInDeadband, ScaleResult::INACURATE_VALUE, 2, "questionable");
}

TEST_F(TestExceptionCalcul, MessagePivotMvSquareRootNeg) 
{
	startTests(jsonMessagePivotMvSquareRootNeg, ScaleResult::INCONSISTENT_VALUE, 0, "questionable");
}

TEST_F(TestExceptionCalcul, MessagePivotMvQuadraNeg) 
{
	startTests(jsonMessagePivotMvQuadraNeg, ScaleResult::INCONSISTENT_VALUE, 0, "questionable");
}

TEST_F(TestExceptionCalcul, MessagePivotMvOverflow) 
{
	startTests(jsonMessagePivotMvOverflow, ScaleResult::OVERFLOW_VALUE, 0, "invalid");
}
