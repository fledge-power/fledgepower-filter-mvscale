#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filterMvScale.h>
#include <jsonToDatapoints.h>
#include <constants.h>

using namespace std;
using namespace DatapointUtility;
using namespace JsonToDatapoints;

static string nameReading = "data_test";

static string jsonMessageValidityInvalid = QUOTE({
	"PIVOTTM": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
            "Identifier": "M_2367_3_15_4",
            "MvTyp": {
                "mag": {
                    "f": 0.1
                },
                "q": {
                    "Source": "process",
                    "Validity": "invalid"
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

static string jsonMessageWithDetailQuality = QUOTE({
	"PIVOTTM": {
        "GTIM": {
            "Cause": {
                "stVal": 1
            },
            "Identifier": "M_2367_3_15_4",
            "MvTyp": {
                "mag": {
                    "f": 0.1
                },
                 "q": {
                    "DetailQuality": {
                        "badReference": 1
                    },
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
	PLUGIN_HANDLE plugin_init(ConfigCategory* config,
			  OUTPUT_HANDLE *outHandle,
			  OUTPUT_STREAM output);
	
	void HandlerStatusPointTimestamping(void *handle, READINGSET *readings) {
		*(READINGSET **)handle = readings;
	}
};

class CheckValidity : public testing::Test
{
protected:
    FilterMvScale * filter = nullptr;  // Object on which we call for tests
    ReadingSet * resultReading;

    // Setup is ran for every tests, so each variable are reinitialised
    void SetUp() override
    {
        PLUGIN_INFORMATION *info = plugin_info();
		ConfigCategory *config = new ConfigCategory("status-points-timestamping", info->config);
		
		ASSERT_NE(config, (ConfigCategory *)NULL);		
		config->setItemsValueFromDefault();
		config->setValue("enable", "true");
		
		void *handle = plugin_init(config, &resultReading, HandlerStatusPointTimestamping);
		filter = (FilterMvScale *) handle;
    }

    // TearDown is ran for every tests, so each variable are destroyed again
    void TearDown() override
    {
        delete filter;
    }

    void startTests(string json) {
        ASSERT_NE(filter, (void *)NULL);

        // Create Reading
        Datapoints * p = parseJson(json);

		Reading* reading = new Reading(nameReading, *p);
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

        verifyDatapoint(&points);

        delete reading;
    }

    void verifyDatapoint(Datapoints * dps) {
        Datapoints * dpPivot = findDictElement(dps, Constants::KEY_MESSAGE_PIVOT_JSON_ROOT);
        ASSERT_NE(dpPivot, nullptr);

        Datapoints * dpGi = findDictElement(dpPivot, Constants::KEY_MESSAGE_PIVOT_JSON_GT);
        ASSERT_NE(dpGi, nullptr);

        Datapoints * mvTyp = findDictElement(dpGi, Constants::KEY_MESSAGE_PIVOT_JSON_CDC_MV);
        ASSERT_NE(mvTyp, nullptr);

		Datapoints * mag = findDictElement(mvTyp, Constants::KEY_MESSAGE_PIVOT_JSON_MAG);
        ASSERT_NE(mag, nullptr);

		DatapointValue * vF = findValueElement(mag, Constants::KEY_MESSAGE_PIVOT_JSON_MAG_F);
		ASSERT_NE(vF, nullptr);
		ASSERT_FLOAT_EQ(vF->toDouble(), 0);

		DatapointValue * vI = findValueElement(mag, Constants::KEY_MESSAGE_PIVOT_JSON_MAG_I);
		ASSERT_EQ(vI, nullptr);

		Datapoints * dpq = findDictElement(mvTyp, Constants::KEY_MESSAGE_PIVOT_JSON_Q);
        ASSERT_NE(dpq, nullptr);

		string validity = findStringElement(dpq, Constants::KEY_MESSAGE_PIVOT_JSON_VALIDITY);
        ASSERT_STREQ(validity.c_str(), "invalid");

		string source = findStringElement(dpq, Constants::KEY_MESSAGE_PIVOT_JSON_SOURCE);
        ASSERT_STREQ(source.c_str(), "process");
    }
};

TEST_F(CheckValidity, ValidityInvalid) 
{
	startTests(jsonMessageValidityInvalid);
}

TEST_F(CheckValidity, DetailQuality) 
{
	startTests(jsonMessageWithDetailQuality);
}
