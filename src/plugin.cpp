/*
 * Fledge filter mvcyclingcheck.
 *
 * Copyright (c) 2020, RTE (https://www.rte-france.com)
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Yannick Marchetaux
 * 
 */
#include <version.h>
#include <filterMvScale.h>
#include <constantsMvScale.h>

static const char *default_config = QUOTE({
		"plugin" : {
			"description" : "Filter that checks the cycling of the measured values",
                       	"type" : "string",
			"default" : FILTER_NAME,
			"readonly" : "true"
			},
		"enable": {
			"description": "A switch that can be used to enable or disable execution of the filter.",
			"displayName": "Enabled",
			"type": "boolean",
			"default": "true"
			},
		
		"exchanged_data" : {
			"description" : "exchanged data list",
			"type" : "JSON",
			"displayName" : "Exchanged data list",
			"order" : "3",
			"default" : QUOTE({
				"exchanged_data": {         
					"datapoints" : [          
						{
							"label":"TM-1",
							"pivot_id":"M_2367_3_15_4",
							"pivot_type":"MvTyp",
							"tfid": "normal",
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
							"label":"TM-2",
							"pivot_id":"M_2367_3_15_5",
							"pivot_type":"MvTyp",
							"tfid": "square_root",
							"params": [1.0, 0.0],
							"deadband" : [1.0, 1.0],
							"protocols":[
								{
									"name":"IEC104",
									"typeid":"M_ME_NC_2",
									"address":"3271644"
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
									"typeid":"M_ME_ND_1",
									"address":"3273333"
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
									"typeid":"M_ME_NB_1",
									"address":"3271444"
								}
							]
						}    
					]
				}
			})
   		}
	});

using namespace std;

/**
 * The Filter plugin interface
 */
extern "C" {

/**
 * The plugin information structure
 */
static PLUGIN_INFORMATION info = {
        FILTER_NAME,              // Name
        VERSION,                  // Version
        0,                        // Flags
        PLUGIN_TYPE_FILTER,       // Type
        "1.0.0",                  // Interface version
		default_config	          // Default plugin configuration
};

struct FILTER_INFO {
	FledgeFilter *handle;
	std::string	configCatName;
};

/**
 * Return the information about this plugin
 */
PLUGIN_INFORMATION *plugin_info() {
	return &info;
}

/**
 * Initialise the plugin, called to get the plugin handle and setup the
 * output handle that will be passed to the output stream. The output stream
 * is merely a function pointer that is called with the output handle and
 * the new set of readings generated by the plugin.
 *     (*output)(outHandle, readings);
 *
 * @param config	The configuration category for the filter
 * @param outHandle	A handle that will be passed to the output stream
 * @param output	The output stream (function pointer) to which data is passed
 * @return		An opaque handle that is used in all subsequent calls to the plugin
 */
PLUGIN_HANDLE plugin_init(ConfigCategory* config,
			  OUTPUT_HANDLE *outHandle,
			  OUTPUT_STREAM output)
{
	auto filterMvScale = new FilterMvScale(FILTER_NAME,
                                        *config,
                                        outHandle,
                                        output);

 	if (config->itemExists("exchanged_data")){
		filterMvScale->setJsonConfig(config->getValue("exchanged_data"));
	}

	return (PLUGIN_HANDLE)filterMvScale;
}

/**
 * Ingest a set of readings into the plugin for processing
 *
 * @param handle	The plugin handle returned from plugin_init
 * @param readingSet	The readings to process
 */
void plugin_ingest(PLUGIN_HANDLE *handle,
		   READINGSET *readingSet)
{
	auto filterMvScale = (FilterMvScale *) handle;
	filterMvScale->ingest(readingSet);
}

/**
 * Plugin reconfiguration entry point
 *
 * @param	handle	The plugin handle
 * @param	newConfig	The new configuration data
 */
void plugin_reconfigure(PLUGIN_HANDLE *handle, const string& newConfig)
{
	auto filterMvScale = (FilterMvScale *)handle;
	filterMvScale->reconfigure(newConfig);
}

/**
 * Call the shutdown method in the plugin
 */
void plugin_shutdown(PLUGIN_HANDLE *handle)
{
	auto filterMvScale = (FilterMvScale *)handle;
	delete filterMvScale;
}

// End of extern "C"
};

