#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filterMvScale.h>
#include <jsonToDatapoints.h>
#include <constants.h>

using namespace std;
using namespace DatapointUtility;
using namespace JsonToDatapoints;

static string nameReading = "data_test";

static string jsonMessagePivotTS = QUOTE({
	"PIVOTTS": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
            "Identifier": "M_2367_3_15_4",
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

static string jsonMessageGiTs = QUOTE({
	"PIVOTTM": {
        "GTIS": {
            "Cause": {
                "stVal": 1
            },
            "Identifier": "M_2367_3_15_4",
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

static string jsonMessageDpsTyp = QUOTE({
	"PIVOTTM": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
            "Identifier": "M_2367_3_15_4",
            "DpsTyp": {
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

static string jsonMessageWithoutMag = QUOTE({
	"PIVOTTM": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
            "Identifier": "M_2367_3_15_4",
            "MvTyp": {
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

static string jsonMessageWithoutID = QUOTE({
	"PIVOTTM": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
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

static string jsonMessageMagEmpty = QUOTE({
	"PIVOTTM": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
			"Identifier": "M_2367_3_15_4",
            "MvTyp": {
                "mag": {
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

static string jsonMessageMagStr = QUOTE({
	"PIVOTTM": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
			"Identifier": "M_2367_3_15_4",
            "MvTyp": {
                "mag": {
					"f" : "string"
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

static string jsonMessageUnknownConfig = QUOTE({
	"PIVOTTM": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
			"Identifier": "unknown",
            "MvTyp": {
                "mag": {
					"f" : 2
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
 
static string jsonMessageWihtoutQ = QUOTE({
	"PIVOTTM": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
			"Identifier": "M_2367_3_15_4",
            "MvTyp": {
                "mag": {
					"f" : 2
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
	PLUGIN_HANDLE plugin_init(ConfigCategory* config,
			  OUTPUT_HANDLE *outHandle,
			  OUTPUT_STREAM output);
	
	void HandlerNoModifyData(void *handle, READINGSET *readings) {
		*(READINGSET **)handle = readings;
	}
};

class NoModifyData : public testing::Test
{
protected:
    FilterMvScale * filter = nullptr;  // Object on which we call for tests
    ReadingSet * resultReading;

    // Setup is ran for every tests, so each variable are reinitialised
    void SetUp() override
    {
        PLUGIN_INFORMATION *info = plugin_info();
		ConfigCategory *config = new ConfigCategory("mvscale", info->config);
		
		ASSERT_NE(config, (ConfigCategory *)NULL);		
		config->setItemsValueFromDefault();
		config->setValue("enable", "true");
		
		void *handle = plugin_init(config, &resultReading, HandlerNoModifyData);
		filter = (FilterMvScale *) handle;
    }

    // TearDown is ran for every tests, so each variable are destroyed again
    void TearDown() override
    {
        delete filter;
    }

	void startTests(string json, std::string namePivotData, std::string nameGi, std::string nameTyp) {
		ASSERT_NE(filter, (void *)NULL);

        // Create Reading
        Datapoints * p = parseJson(json);

		Reading *reading = new Reading(nameReading, *p);
        Readings *readings = new Readings;
        readings->push_back(reading);

        // Create ReadingSet
        ReadingSet * readingSet = new ReadingSet(readings);
		
        plugin_ingest(filter, (READINGSET*)readingSet);
        Readings results = resultReading->getAllReadings();
        ASSERT_EQ(results.size(), 1);

        Reading *out = results[0];
        ASSERT_STREQ(out->getAssetName().c_str(), nameReading.c_str());
        ASSERT_EQ(out->getDatapointCount(), 1);

        Datapoints points = out->getReadingData();
        ASSERT_EQ(points.size(), 1);

        verifyDatapoint(&points, namePivotData, nameGi, nameTyp, json);

        delete reading;
	}

	void verifyDatapoint(Datapoints *dps, std::string namePivotData, std::string nameGi, std::string nameTyp, string json) {
		Datapoints *dpPivot = findDictElement(dps, namePivotData);
		ASSERT_NE(dpPivot, nullptr);
		
		Datapoints *dpGi = findDictElement(dpPivot, nameGi);
		ASSERT_NE(dpGi, nullptr);
		
		Datapoints *dpTyp = findDictElement(dpGi, nameTyp);
		ASSERT_NE(dpTyp, nullptr);
		
		if (json != jsonMessageWithoutMag && json != jsonMessageMagEmpty && json != jsonMessageMagStr) {
			Datapoints *dpMag = findDictElement(dpTyp, Constants::KEY_MESSAGE_PIVOT_JSON_MAG);
			DatapointValue *valueMv = nullptr;
			if (dpMag != nullptr) {
				valueMv = findValueElement(dpMag, Constants::KEY_MESSAGE_PIVOT_JSON_MAG_I);
				if (valueMv == nullptr) {
					valueMv = findValueElement(dpMag, Constants::KEY_MESSAGE_PIVOT_JSON_MAG_F);
				}
			}
			ASSERT_NE(valueMv, nullptr);

			if (valueMv->getType() == DatapointValue::T_INTEGER) {
				ASSERT_DOUBLE_EQ(valueMv->toInt(), 2);
			}
			else if (valueMv->getType() == DatapointValue::T_FLOAT) {
				ASSERT_DOUBLE_EQ(valueMv->toDouble(), 2);
			}
		}

		Datapoints *dpQ = findDictElement(dpTyp, Constants::KEY_MESSAGE_PIVOT_JSON_Q);
		if (json == jsonMessageWihtoutQ) {	
			ASSERT_EQ(dpQ, nullptr);
		}
		else {
			ASSERT_NE(dpQ, nullptr);
			DatapointValue *vSource = findValueElement(dpQ, Constants::KEY_MESSAGE_PIVOT_JSON_SOURCE);
			ASSERT_NE(vSource, nullptr);
			ASSERT_STREQ(vSource->toStringValue().c_str(), "process"); 
		}
	}
};

TEST_F(NoModifyData, MessagePIVOTTS) 
{
	startTests(jsonMessagePivotTS, "PIVOTTS", "GTIM", "MvTyp");
}

TEST_F(NoModifyData, MessageGTIS) 
{
	startTests(jsonMessageGiTs, "PIVOTTM", "GTIS", "MvTyp");
}

TEST_F(NoModifyData, MessageDSPTYP) 
{
	startTests(jsonMessageDpsTyp, "PIVOTTM", "GTIM", "DpsTyp");
}

TEST_F(NoModifyData, MessageWithoutMag) 
{
	startTests(jsonMessageWithoutMag, "PIVOTTM", "GTIM", "MvTyp");
}

TEST_F(NoModifyData, MessageWithoutID) 
{
	startTests(jsonMessageWithoutID, "PIVOTTM", "GTIM", "MvTyp");
}

TEST_F(NoModifyData, MessageMagEmpty) 
{
	startTests(jsonMessageMagEmpty, "PIVOTTM", "GTIM", "MvTyp");
}

TEST_F(NoModifyData, MessageMagStr) 
{
	startTests(jsonMessageMagStr, "PIVOTTM", "GTIM", "MvTyp");
}

TEST_F(NoModifyData, MessageUnknownConfig) 
{
	startTests(jsonMessageUnknownConfig, "PIVOTTM", "GTIM", "MvTyp");
}

TEST_F(NoModifyData, MessageWihtoutQ) 
{
	startTests(jsonMessageWihtoutQ, "PIVOTTM", "GTIM", "MvTyp");
}
