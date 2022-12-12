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
                        "params": [2.0, 1.0],
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
                        "label":"TM-2",
                        "pivot_id":"M_2367_3_15_5",
                        "pivot_type":"MvTyp",
                        "tfid": "square_root",
                        "params": [3.0, 1.0],
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
                        "params": [2.0, 2.0],
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
                        "tfid": "transparent",
                        "params": [1.0, 0.0],
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

static string jsonMessagePivotMVNormal = QUOTE({
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

static string jsonMessagePivotMVSquareRoot = QUOTE({
	"PIVOTTM": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
            "Identifier": "M_2367_3_15_5",
            "MvTyp": {
                "mag": {
                    "f": 4
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

static string jsonMessagePivotMVQuadra = QUOTE({
	"PIVOTTM": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
            "Identifier": "M_2367_3_15_6",
            "MvTyp": {
                "mag": {
                    "f": 1
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

static string jsonMessagePivotMVTransparent = QUOTE({
	"PIVOTTM": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
            "Identifier": "M_2367_3_15_7",
            "MvTyp": {
                "mag": {
                    "f": 2
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
	
	void HandlerTestScaleMV(void *handle, READINGSET *readings) {
		*(READINGSET **)handle = readings;
	}

    void plugin_reconfigure(PLUGIN_HANDLE *handle, const string& newConfig);
};

class TestScaleMV : public testing::Test
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
		
		void *handle = plugin_init(config, &resultReading, HandlerTestScaleMV);
		filter = (FilterMvScale *) handle;

        plugin_reconfigure((PLUGIN_HANDLE*)filter, configure);
    }

    // TearDown is ran for every tests, so each variable are destroyed again
    void TearDown() override
    {
        delete filter;
    }

	void startTests(string json, ScaleType type) {
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

        verifyDatapoint(&points, type);
        delete reading;
	}

	void verifyDatapoint(Datapoints *dps, ScaleType type) {
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

        float f = 0;
        switch (type) {
            case ScaleType::NORMAL:
                f = 2 * 2 + 1;
                break;
            case ScaleType::SQUARE_ROOT:
                f = 3 * sqrt(4) + 1;
                break;
            case ScaleType::QUADRATIC:
                f = sqrt((2 * 1) + 2);
                break;
            case ScaleType::TRANSPARENT:
            default:
                f = 2;
                break;
        }

        ASSERT_FLOAT_EQ(valueMv->toDouble(), f);

		Datapoints *dpQ = findDictElement(dpTyp, ConstantsMvScale::KeyMessagePivotJsonQ);
		ASSERT_NE(dpQ, nullptr);

        Datapoints *dpDetailQuality = findDictElement(dpQ, ConstantsMvScale::KeyMessagePivotJsonDetailQuality);
		ASSERT_EQ(dpDetailQuality, nullptr);

		DatapointValue *vSource = findValueElement(dpQ, ConstantsMvScale::KeyMessagePivotJsonSource);
		ASSERT_NE(vSource, nullptr);

        if (type != ScaleType::TRANSPARENT) {
		    ASSERT_STREQ(vSource->toStringValue().c_str(), "substituted"); 
        }
        else {
		    ASSERT_STREQ(vSource->toStringValue().c_str(), "process"); 
        }
	}
};

TEST_F(TestScaleMV, MessagePivotMVNormal) 
{
	startTests(jsonMessagePivotMVNormal, ScaleType::NORMAL);
}

TEST_F(TestScaleMV, MessagePivotMVSquareRoot) 
{
	startTests(jsonMessagePivotMVSquareRoot, ScaleType::SQUARE_ROOT);
}

TEST_F(TestScaleMV, MessagePivotMVQuadra) 
{
	startTests(jsonMessagePivotMVQuadra, ScaleType::QUADRATIC);
}

TEST_F(TestScaleMV, MessagePivotMVTransparent) 
{
	startTests(jsonMessagePivotMVTransparent, ScaleType::TRANSPARENT);
}