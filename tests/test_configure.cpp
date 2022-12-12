#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filterMvScale.h>

using namespace std;

static string reconfigureErrorParseJSON = QUOTE({
    "exchanged_data" : {
        "eee"
    });

static string configureErrorExchanged_data = QUOTE({
    "configureErrorExchanged_data": {         
        "datapoints" : [          
            {
                "label":"TM-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":"MvTyp",
                "tfid": "normal",
                "params": [2.0, 1],
                "deadband" : [1.0, 1.0]
            }
        ]
    }
});

static string configureErrorDatapoint = QUOTE({
    "exchanged_data": {         
        "configureErrorDatapoint" : [          
            {
                "label":"TM-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":"MvTyp",
                "tfid": "normal",
                "params": [2.0, 1],
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
});

static string configureErrorType = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TS-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":1
            }
        ]
    }
});

static string configureErrorTypeSps = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TS-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":"SpsTyp"
            }
        ]
    }
});

static string configureErrorPivotID = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TM-1",
                "pivot_id":1,
                "pivot_type":"MvTyp",
                "tfid": "normal",
                "params": [2.0, 1],
                "deadband" : [1.0, 1.0]
            }
        ]
    }
});

static string configureErrorTFID = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TM-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":"MvTyp",
                "params": [2.0, 1],
                "deadband" : [1.0, 1.0]
            }
        ]
    }
});

static string configureErrorFactors = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TM-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":"MvTyp",
                "tfid": "normal",
                "params": [2.0],
                "deadband" : [1.0, 1.0]
            }
        ]
    }
});

static string configureErrorDeadband = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TM-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":"MvTyp",
                "tfid": "normal",
                "params": [2.0, 1]
            }
        ]
    }
});

static string configureErrorParams = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TM-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":"MvTyp",
                "tfid": "normal",
                "deadband": [2.0, 1]
            }
        ]
    }
});

static string configureFactorA0 = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TM-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":"MvTyp",
                "tfid": "normal",
                "params": [0, 1],
                "deadband" : [1.0, 1.0]
            }
        ]
    }
});

static string configureOK = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TM-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":"MvTyp",
                "tfid": "normal",
                "params": [2.4, 3.6],
                "deadband" : [4.2, 5.5]
            }
        ]
    }
});

static string configureSquareRoot = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TM-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":"MvTyp",
                "tfid": "square_root",
                "params": [2.4, 3.6],
                "deadband" : [4.2, 5.5]
            }
        ]
    }
});

static string configureQuadra = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TM-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":"MvTyp",
                "tfid": "quadratic",
                "params": [2.4, 3.6],
                "deadband" : [4.2, 5.5]
            }
        ]
    }
});

static string configureTransparent = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TM-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":"MvTyp",
                "tfid": "transparent",
                "params": [2.4, 3.6],
                "deadband" : [4.2, 5.5]
            }
        ]
    }
});

extern "C" {
	PLUGIN_INFORMATION *plugin_info();
	PLUGIN_HANDLE plugin_init(ConfigCategory *config,
			  OUTPUT_HANDLE *outHandle,
			  OUTPUT_STREAM output);
};

class PluginConfigure : public testing::Test
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
		
		void *handle = plugin_init(config, &resultReading, nullptr);
		filter = (FilterMvScale *) handle;
    }

    // TearDown is ran for every tests, so each variable are destroyed again
    void TearDown() override
    {
        delete filter;
    }   
};

TEST_F(PluginConfigure, ConfigureErrorParsingJSON) 
{
	filter->setJsonConfig(reconfigureErrorParseJSON);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition().size(), 0);
}

TEST_F(PluginConfigure, ConfigureErrorExchanged_data) 
{
	filter->setJsonConfig(configureErrorExchanged_data);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition().size(), 0);
}

TEST_F(PluginConfigure, ConfigureErrorDatapoint) 
{
	filter->setJsonConfig(configureErrorDatapoint);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition().size(), 0);
}

TEST_F(PluginConfigure, ConfigureErrorType) 
{
	filter->setJsonConfig(configureErrorType);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition().size(), 0);
}

TEST_F(PluginConfigure, ConfigureErrorTypeSps) 
{
	filter->setJsonConfig(configureErrorTypeSps);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition().size(), 0);
}

TEST_F(PluginConfigure, ConfigureErrorPivotID) 
{
	filter->setJsonConfig(configureErrorPivotID);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition().size(), 0);
}

TEST_F(PluginConfigure, ConfigureErrorTFID) 
{
	filter->setJsonConfig(configureErrorTFID);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition().size(), 0);
}

TEST_F(PluginConfigure, ConfigureErrorFactors) 
{
	filter->setJsonConfig(configureErrorFactors);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition().size(), 0);
}

TEST_F(PluginConfigure, ConfigureErrorDeadband) 
{
	filter->setJsonConfig(configureErrorDeadband);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition().size(), 0);
}

TEST_F(PluginConfigure, ConfigureErrorParams) 
{
	filter->setJsonConfig(configureErrorParams);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition().size(), 0);
}

TEST_F(PluginConfigure, ConfigureFactorA0) 
{
	filter->setJsonConfig(configureFactorA0);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition().size(), 1);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition()["M_2367_3_15_4"]->factorA, 1);
}

TEST_F(PluginConfigure, ConfigureOK) 
{
	filter->setJsonConfig(configureOK);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition().size(), 1);
    ASSERT_FLOAT_EQ(filter->getConfig()->getDataExchangeWithID("M_2367_3_15_4")->factorA, 2.4);
    ASSERT_FLOAT_EQ(filter->getConfig()->getDataExchangeWithID("M_2367_3_15_4")->factorB, 3.6);
    ASSERT_FLOAT_EQ(filter->getConfig()->getDataExchangeWithID("M_2367_3_15_4")->deadbandMin, 4.2);
    ASSERT_FLOAT_EQ(filter->getConfig()->getDataExchangeWithID("M_2367_3_15_4")->deadbandMax, 5.5);
    ASSERT_EQ(filter->getConfig()->getDataExchangeWithID("M_2367_3_15_4")->typeScale, ScaleType::NORMAL);
}


TEST_F(PluginConfigure, ConfigureSquareRoot) 
{
	filter->setJsonConfig(configureSquareRoot);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition().size(), 1);
    ASSERT_FLOAT_EQ(filter->getConfig()->getExchangeDefinition()["M_2367_3_15_4"]->factorA, 2.4);
    ASSERT_FLOAT_EQ(filter->getConfig()->getExchangeDefinition()["M_2367_3_15_4"]->factorB, 3.6);
    ASSERT_FLOAT_EQ(filter->getConfig()->getExchangeDefinition()["M_2367_3_15_4"]->deadbandMin, 4.2);
    ASSERT_FLOAT_EQ(filter->getConfig()->getExchangeDefinition()["M_2367_3_15_4"]->deadbandMax, 5.5);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition()["M_2367_3_15_4"]->typeScale, ScaleType::SQUARE_ROOT);
}

TEST_F(PluginConfigure, ConfigureQuadra) 
{
	filter->setJsonConfig(configureQuadra);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition().size(), 1);
    ASSERT_FLOAT_EQ(filter->getConfig()->getExchangeDefinition()["M_2367_3_15_4"]->factorA, 2.4);
    ASSERT_FLOAT_EQ(filter->getConfig()->getExchangeDefinition()["M_2367_3_15_4"]->factorB, 3.6);
    ASSERT_FLOAT_EQ(filter->getConfig()->getExchangeDefinition()["M_2367_3_15_4"]->deadbandMin, 4.2);
    ASSERT_FLOAT_EQ(filter->getConfig()->getExchangeDefinition()["M_2367_3_15_4"]->deadbandMax, 5.5);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition()["M_2367_3_15_4"]->typeScale, ScaleType::QUADRATIC);
}

TEST_F(PluginConfigure, ConfigureTransparent) 
{
	filter->setJsonConfig(configureTransparent);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition().size(), 1);
    ASSERT_FLOAT_EQ(filter->getConfig()->getExchangeDefinition()["M_2367_3_15_4"]->factorA, 2.4);
    ASSERT_FLOAT_EQ(filter->getConfig()->getExchangeDefinition()["M_2367_3_15_4"]->factorB, 3.6);
    ASSERT_FLOAT_EQ(filter->getConfig()->getExchangeDefinition()["M_2367_3_15_4"]->deadbandMin, 4.2);
    ASSERT_FLOAT_EQ(filter->getConfig()->getExchangeDefinition()["M_2367_3_15_4"]->deadbandMax, 5.5);
    ASSERT_EQ(filter->getConfig()->getExchangeDefinition()["M_2367_3_15_4"]->typeScale, ScaleType::TRANSPARENT);
}