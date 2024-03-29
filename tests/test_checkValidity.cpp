#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filterMvScale.h>
#include <constantsMvScale.h>

using namespace std;
using namespace DatapointUtility;

static string nameReading = "data_test";

static string jsonMessageValidityInvalid = QUOTE({
	"PIVOT": {
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
	"PIVOT": {
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
	PLUGIN_HANDLE plugin_init(ConfigCategory *config,
			  OUTPUT_HANDLE *outHandle,
			  OUTPUT_STREAM output);
	
	void HandlerStatusPointTimestamping(void *handle, READINGSET *readings) {
		*(READINGSET **)handle = readings;
	}
};

class CheckValidity : public testing::Test
{
protected:
    FilterMvScale *filter = nullptr;  // Object on which we call for tests
    ReadingSet *resultReading;

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
        DatapointValue d("");
        Datapoint *tmp = new Datapoint("", d);
        Datapoints *p = tmp->parseJson(json);
        delete tmp;

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

        verifyDatapoint(&points);

        delete reading;
    }

    void verifyDatapoint(Datapoints *dps) {
        Datapoints *dpPivot = findDictElement(dps, ConstantsMvScale::KeyMessagePivotJsonRoot);
        ASSERT_NE(dpPivot, nullptr);

        Datapoints *dpGi = findDictElement(dpPivot, ConstantsMvScale::KeyMessagePivotJsonGt);
        ASSERT_NE(dpGi, nullptr);

        Datapoints *mvTyp = findDictElement(dpGi, ConstantsMvScale::KeyMessagePivotJsonCdcMv);
        ASSERT_NE(mvTyp, nullptr);

		Datapoints *mag = findDictElement(mvTyp, ConstantsMvScale::KeyMessagePivotJsonMag);
        ASSERT_NE(mag, nullptr);

		DatapointValue *vF = findValueElement(mag, ConstantsMvScale::KeyMessagePivotJsonMagF);
		ASSERT_NE(vF, nullptr);
		ASSERT_FLOAT_EQ(vF->toDouble(), 0);

		DatapointValue *vI = findValueElement(mag, ConstantsMvScale::KeyMessagePivotJsonMagI);
		ASSERT_EQ(vI, nullptr);

		Datapoints *dpq = findDictElement(mvTyp, ConstantsMvScale::KeyMessagePivotJsonQ);
        ASSERT_NE(dpq, nullptr);

		string validity = findStringElement(dpq, ConstantsMvScale::KeyMessagePivotJsonValidity);
        ASSERT_STREQ(validity.c_str(), "invalid");

		string source = findStringElement(dpq, ConstantsMvScale::KeyMessagePivotJsonSource);
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
