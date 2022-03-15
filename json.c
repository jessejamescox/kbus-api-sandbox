// Copyright <2022> <Jesse Cox>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files (the "Software"), to deal in the Software
// without restriction, including without limitation the rights to use, copy, modify,
// merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following:

// The above copyright notice and this permission notice shall be included in all copies
// or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
// CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>
#include "json-c/json.h"
#include "kbus-api.h"
#include "kbus.h"
#include "json.h"
#include "node.h"
#include "get_config.h"
#include "utils.h"
#include <time.h>
#include <string.h>

#define MAX_IMG_LENGTH 2048

// helper functions
struct json_object *dig_in_channel_object(int mp, int cp)
{
	struct json_object *jobj = json_object_new_object();
	json_object_object_add(jobj, "value", json_object_new_boolean(diMod[controller.modules[mp].typeIndex].inData[cp].value));
	json_object_object_add(jobj, "label", json_object_new_string(diMod[controller.modules[mp].typeIndex].inData[cp].label));
	return jobj;
};

struct json_object *dig_out_channel_object(int mp, int cp)
{
	struct json_object *jobj = json_object_new_object();
	json_object_object_add(jobj, "value", json_object_new_boolean(doMod[controller.modules[mp].typeIndex].outData[cp].value));
	json_object_object_add(jobj, "label", json_object_new_string(doMod[controller.modules[mp].typeIndex].outData[cp].label));

	return jobj;
}

struct json_object *dx_in_channel_object(int mp, int cp)
{
	struct json_object *jobj = json_object_new_object();
	json_object_object_add(jobj, "value", json_object_new_boolean(dxMod[controller.modules[mp].typeIndex].inData[cp].value));
	json_object_object_add(jobj, "label", json_object_new_string(dxMod[controller.modules[mp].typeIndex].inData[cp].label));
	return jobj;
};

struct json_object *dx_out_channel_object(int mp, int cp)
{
	struct json_object *jobj = json_object_new_object();
	json_object_object_add(jobj, "value", json_object_new_boolean(dxMod[controller.modules[mp].typeIndex].outData[cp].value));
	json_object_object_add(jobj, "label", json_object_new_string(dxMod[controller.modules[mp].typeIndex].outData[cp].label));

	return jobj;
};

struct json_object *analog_in_channel_object(int mp, int cp)
{
	struct json_object *jobj = json_object_new_object();
	json_object_object_add(jobj, "value", json_object_new_int(aiMod[controller.modules[mp].typeIndex].inData[cp].value));
	json_object_object_add(jobj, "label", json_object_new_string(aiMod[controller.modules[mp].typeIndex].inData[cp].label));
	json_object_object_add(jobj, "deadband", json_object_new_int(aiMod[controller.modules[mp].typeIndex].inData[cp].deadband));
	return jobj;
}

struct json_object *analog_out_channel_object(int mp, int cp)
{
	struct json_object *jobj = json_object_new_object();
	json_object_object_add(jobj, "value", json_object_new_int(aoMod[controller.modules[mp].typeIndex].outData[cp].value));
	json_object_object_add(jobj, "label", json_object_new_string(aoMod[controller.modules[mp].typeIndex].outData[cp].label));
	return jobj;
}

struct json_object *pmm_module_object(int mp)
{
	struct json_object *jobj = json_object_new_object();
	struct json_object *jsL1 = json_object_new_object();
	struct json_object *jsL2 = json_object_new_object();
	struct json_object *jsL3 = json_object_new_object();

	json_object_object_add(jsL1, "voltage", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L1.measuredVolts * 0.01) * 100) / 100));
	json_object_object_add(jsL1, "current", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L1.measuredAmps * 0.0001) * 100) / 100));
	json_object_object_add(jsL1, "power", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L1.measuredPower * 0.01) * 100) / 100));
	json_object_object_add(jsL1, "frequency", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L1.measuredFrequency * 0.001) * 100) / 100));

	json_object_object_add(jsL1, "errorGeneral", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L1.errorGeneral));

	/*json_object_object_add(jsL1, "errorUnderVolts", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L1.errorUnderVolts)) ;
	json_object_object_add(jsL1, "errorOverVolts", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L1.errorOverVolts)) ;
	json_object_object_add(jsL1, "errorCurrentRange", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L1.errorCurrentRange)) ;
	json_object_object_add(jsL1, "errorOverCurrent", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L1.errorOverCurrent)) ;
	json_object_object_add(jsL1, "errorVoltsRange", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L1.errorVoltsRange)) ;
	json_object_object_add(jsL1, "errorFieldCCW", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L1.errorFieldCCW)) ;
	json_object_object_add(jsL1, "errorZeroCross", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L1.errorZeroCross)) ;*/

	json_object_object_add(jsL2, "voltage", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L2.measuredVolts * 0.01) * 100) / 100));
	json_object_object_add(jsL2, "current", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L2.measuredAmps * 0.0001) * 100) / 100));
	json_object_object_add(jsL2, "power", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L2.measuredPower * 0.01) * 100) / 100));
	json_object_object_add(jsL2, "frequency", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L2.measuredFrequency * 0.001) * 100) / 100));

	json_object_object_add(jsL2, "errorGeneral", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L2.errorGeneral));

	/*json_object_object_add(jsL2, "errorUnderVolts", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L2.errorUnderVolts)) ;
	json_object_object_add(jsL2, "errorOverVolts", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L2.errorOverVolts)) ;
	json_object_object_add(jsL2, "errorCurrentRange", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L2.errorCurrentRange)) ;
	json_object_object_add(jsL2, "errorOverCurrent", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L2.errorOverCurrent)) ;
	json_object_object_add(jsL2, "errorVoltsRange", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L2.errorVoltsRange)) ;
	json_object_object_add(jsL2, "errorFieldCCW", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L2.errorFieldCCW)) ;
	json_object_object_add(jsL2, "errorZeroCross", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L2.errorZeroCross)) ;*/

	json_object_object_add(jsL3, "voltage", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L3.measuredVolts * 0.01) * 100) / 100));
	json_object_object_add(jsL3, "current", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L3.measuredAmps * 0.0001) * 100) / 100));
	json_object_object_add(jsL3, "power", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L3.measuredPower * 0.01) * 100) / 100));
	json_object_object_add(jsL3, "frequency", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L3.measuredFrequency * 0.001) * 100) / 100));

	json_object_object_add(jsL3, "errorGeneral", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L3.errorGeneral));

	/*json_object_object_add(jsL3, "errorUnderVolts", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L3.errorUnderVolts)) ;
	json_object_object_add(jsL3, "errorOverVolts", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L3.errorOverVolts)) ;
	json_object_object_add(jsL3, "errorCurrentRange", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L3.errorCurrentRange)) ;
	json_object_object_add(jsL3, "errorOverCurrent", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L3.errorOverCurrent)) ;
	json_object_object_add(jsL3, "errorVoltsRange", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L3.errorVoltsRange)) ;
	json_object_object_add(jsL3, "errorFieldCCW", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L3.errorFieldCCW)) ;
	json_object_object_add(jsL3, "errorZeroCross", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L3.errorZeroCross)) ;*/

	json_object_object_add(jobj, "L1", json_object_get(jsL1));
	json_object_object_add(jobj, "L2", json_object_get(jsL2));
	json_object_object_add(jobj, "L3", json_object_get(jsL3));
	json_object_object_add(jobj, "errorGroup", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].grpError));
	json_object_object_add(jobj, "errorGeneral", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].genError));

	json_object_put(jsL1);
	json_object_put(jsL2);
	json_object_put(jsL3);

	return jobj;
}

struct json_object *in_data_breakout_object(int mp)
{
	struct json_object *jobj = json_object_new_object();

	for (int channelIndex = 0; channelIndex < controller.modules[mp].outChannelCount; channelIndex++)
	{
		char *chn = (char *)malloc(10 * sizeof(char));
		strcpy(chn, "channel");
		char *ch;
		itoa((channelIndex + 1), &ch, 10);
		strcat(chn, &ch);

		json_object_object_add(jobj, chn, dx_in_channel_object(mp, channelIndex));

		free(chn);
	}
	return jobj;
}

struct json_object *out_data_breakout_object(int mp)
{
	struct json_object *jobj = json_object_new_object();

	for (int channelIndex = 0; channelIndex < controller.modules[mp].outChannelCount; channelIndex++)
	{
		char *chn = (char *)malloc(10 * sizeof(char));
		strcpy(chn, "channel");
		char *ch;
		itoa((channelIndex + 1), &ch, 10);
		strcat(chn, &ch);

		json_object_object_add(jobj, chn, dx_out_channel_object(mp, channelIndex));

		free(chn);
	}
	return jobj;
}

struct json_object *simple_channels_object(int mp)
{
	struct json_object *jobj = json_object_new_object();

	switch (controller.modules[mp].mtype)
	{
	case dim: // digital inputs
		for (int channelIndex = 0; channelIndex < controller.modules[mp].channelCount; channelIndex++)
		{
			char *chn = (char *)malloc(10 * sizeof(char));
			strcpy(chn, "channel");
			char *ch;
			itoa((channelIndex + 1), &ch, 10);
			strcat(chn, &ch);

			json_object_object_add(jobj, chn, dig_in_channel_object(mp, channelIndex));

			free(chn);
		}
		break;

	case dom:

		for (int channelIndex = 0; channelIndex < controller.modules[mp].channelCount; channelIndex++)
		{
			char *chn = (char *)malloc(10 * sizeof(char));
			strcpy(chn, "channel");
			char *ch;
			itoa((channelIndex + 1), &ch, 10);
			strcat(chn, &ch);

			json_object_object_add(jobj, chn, dig_out_channel_object(mp, channelIndex));

			free(chn);
		}
		break;

	case dxm:
		json_object_object_add(jobj, "inputs", in_data_breakout_object(mp));
		json_object_object_add(jobj, "outputs", out_data_breakout_object(mp));
		break;

	case aim:
		for (int channelIndex = 0; channelIndex < controller.modules[mp].inChannelCount; channelIndex++)
		{
			char *chn = (char *)malloc(10 * sizeof(char));
			strcpy(chn, "channel");
			char *ch;
			itoa((channelIndex + 1), &ch, 10);
			strcat(chn, &ch);

			json_object_object_add(jobj, chn, analog_in_channel_object(mp, channelIndex));

			free(chn);
		}
		break;

	case aom:
		for (int channelIndex = 0; channelIndex < controller.modules[mp].channelCount; channelIndex++)
		{
			char *chn = (char *)malloc(10 * sizeof(char));
			strcpy(chn, "channel");
			char *ch;
			itoa((channelIndex + 1), &ch, 10);
			strcat(chn, &ch);

			json_object_object_add(jobj, chn, analog_out_channel_object(mp, channelIndex));

			free(chn);
		}
		break;
	}
	return jobj;
};

struct json_object *simple_module_object(int mp)
{
	struct json_object *jobj = json_object_new_object();

	// add the module info
	json_object_object_add(jobj, "pn", json_object_new_int(controller.modules[mp].pn));
	json_object_object_add(jobj, "position", json_object_new_int(controller.modules[mp].position));
	json_object_object_add(jobj, "type", json_object_new_string(controller.modules[mp].type));

	if (controller.modules[mp].mtype == spm)
	{
		switch (controller.modules[mp].pn)
		{
		case 494 ... 495: // check for power measurement module
			json_object_object_add(jobj, "data", pmm_module_object(mp));
			break;
		}
	}
	else // somple modules
	{
		json_object_object_add(jobj, "input_channel_count", json_object_new_int(controller.modules[mp].inChannelCount));
		json_object_object_add(jobj, "output_channel_count", json_object_new_int(controller.modules[mp].outChannelCount));

		// add the channel info
		json_object_object_add(jobj, "channels", simple_channels_object(mp));
	}

	return jobj;
}

struct json_object *simple_modules_object()
{
	struct json_object *jobj = json_object_new_object();

	// add the channels
	for (int moduleIndex = 0; moduleIndex < controller.number_of_modules; moduleIndex++)
	{

		// build the channel object key
		char *mod = (char *)malloc(10 * sizeof(char));
		strcpy(mod, "module");
		char *mi;
		itoa((moduleIndex + 1), mi, 10);
		strcat(mod, mi);

		// build the objects in a loop
		json_object_object_add(jobj, mod, simple_module_object(moduleIndex));

		// free the holding char
		free(mod);
	}
	return jobj;
}

struct json_object *main_controller_object()
{
	struct json_object *jobj = json_object_new_object();

	// add the module info
	json_object_object_add(jobj, "node_id", json_object_new_string(controller.nodeId));
	json_object_object_add(jobj, "switch_state", json_object_new_string(controller.switch_state));
	json_object_object_add(jobj, "module_count", json_object_new_int(controller.number_of_modules));

	// add the channel info
	json_object_object_add(jobj, "modules", simple_modules_object());

	return jobj;
}

void build_controller_object(struct mosquitto *mosq)
{
	struct json_object *jsState = json_object_new_object();
	struct json_object *jsReported = json_object_new_object();
	// struct json_object *jsController = json_object_new_object();s

	// json_object_object_add(jsController, "controller", main_controller_object());
	json_object_object_add(jsReported, "reported", main_controller_object());
	json_object_object_add(jsState, "state", jsReported);

	// char *jsonString = (char *) malloc(4096 * sizeof(char));
	char *jsonString = json_object_to_json_string(jsState);

	int msq_pub = mosquitto_publish(mosq, NULL, this_config.status_pub_topic, strlen(jsonString), jsonString, 0, 0);

	// while (json_object_put(jsController)) {};
	while (json_object_put(jsReported))
	{
	};
	while (json_object_put(jsState))
	{
	};
}

void build_event_object(struct mosquitto *mosq, int modulePosition, int channelPosition, int channelValue)
{

	char *chn = (char *)malloc(10);
	char *mod = (char *)malloc(10);

	strcpy(mod, "module");
	char *mi;
	itoa((modulePosition + 1), mi, 10);
	strcat(mod, mi);

	strcpy(chn, "channel");
	char *ch;
	itoa((channelPosition + 1), ch, 10);
	strcat(chn, ch);

	struct json_object *jsonMain = json_object_new_object();
	struct json_object *jsonState = json_object_new_object();
	struct json_object *jsonReported = json_object_new_object();
	struct json_object *jsonController = json_object_new_object();
	struct json_object *jsonModules = json_object_new_object();
	struct json_object *jsonModule = json_object_new_object();
	struct json_object *jsonChannels = json_object_new_object();
	struct json_object *jsonChannel = json_object_new_object();
	//*jsonState, *jsonReported, *jsonController, *jsonModules, *jsonModule, * jsonChannels, *jsonChannel = json_object_new_object();

	// build the channel object
	if (!strcmp(controller.modules[modulePosition].type, "DI"))
	{
		json_object_object_add(jsonChannel, "value", json_object_new_boolean(channelValue));
	}
	if (!strcmp(controller.modules[modulePosition].type, "DO"))
	{
		json_object_object_add(jsonChannel, "value", json_object_new_boolean(channelValue));
	}
	if (!strcmp(controller.modules[modulePosition].type, "AI"))
	{
		json_object_object_add(jsonChannel, "value", json_object_new_int(channelValue));
	}
	if (!strcmp(controller.modules[modulePosition].type, "AO"))
	{
		json_object_object_add(jsonChannel, "value", json_object_new_int(channelValue));
	}

	// build the module object
	json_object_object_add(jsonChannels, chn, json_object_get(jsonChannel));

	json_object_object_add(jsonModule, "channels", json_object_get(jsonChannels));

	// add this to the reported object
	json_object_object_add(jsonModules, mod, json_object_get(jsonModule));

	// add this to the reported object
	json_object_object_add(jsonController, "node_id", json_object_new_string(controller.nodeId));
	json_object_object_add(jsonController, "switch_state", json_object_new_int(controller.switch_state)); // faulting here?? changed this to int
	json_object_object_add(jsonController, "modules", json_object_get(jsonModules));

	// add this to the reported object
	json_object_object_add(jsonReported, "controller", json_object_get(jsonController));

	// add the module to the reported
	json_object_object_add(jsonState, "reported", json_object_get(jsonReported));

	// add the reported to the state
	json_object_object_add(jsonMain, "state", json_object_get(jsonState));

	char *event_string = json_object_to_json_string(jsonMain);

	int pub_resp = mosquitto_publish(mosq, NULL, this_config.event_pub_topic, strlen(event_string), event_string, 0, 0);

	free(mod);
	free(chn);

	// free the owner
	json_object_put(jsonChannel);
	json_object_put(jsonChannels);
	json_object_put(jsonModule);
	json_object_put(jsonModules);
	json_object_put(jsonController);
	json_object_put(jsonReported);
	json_object_put(jsonState);
	json_object_put(jsonMain);
}

int parse_mqtt(struct mosquitto *mosq, char *message)
{

	struct channel_command channelCmd;

	struct json_object *jsonState, *jsonDesired, *jsonController, *jsonModules, *jsonModule, *jsonChannels, *jsonChannel, *jsonValue, *jsonLabel;

	int pjFree, jhFree, jh2Free;

	// char *mod, *chn;
	char *mod = (char *)malloc(10 * sizeof(char));
	char *chn = (char *)malloc(10 * sizeof(char));

	// get the main json object
	struct json_object *parsed_json = json_tokener_parse(message);

	// start checking the json objects
	if (json_object_object_get_ex(parsed_json, "state", &jsonState))
	{
		if (json_object_object_get_ex(jsonState, "desired", &jsonDesired))
		{
			if (json_object_object_get_ex(jsonDesired, "modules", &jsonModules))
			{

				for (int iModules = 0; iModules < controller.number_of_modules; iModules++)
				{
					mod = (char *)malloc(10 * sizeof(char));

					strcpy(mod, "module");
					char *mi;
					itoa((iModules + 1), &mi, 10);
					strcat(mod, &mi);

					// find the object
					if (json_object_object_get_ex(jsonModules, mod, &jsonModule))
					{

						// found the channel, record the module position
						channelCmd.module = iModules;

						if (json_object_object_get_ex(jsonModule, "channels", &jsonChannels))
						{

							// search for the channels
							for (int iChannels = 0; iChannels < controller.modules[iModules].channelCount; iChannels++)
							{
								// chn = (char*)malloc(10 * sizeof(char));

								strcpy(chn, "channel");
								char *ch;
								itoa((iChannels + 1), &ch, 10);
								strcat(chn, &ch);

								// find the object
								if (json_object_object_get_ex(jsonChannels, chn, &jsonChannel))
								{

									// found the channel, record the module position
									channelCmd.channel = iChannels;

									if (json_object_object_get_ex(jsonChannel, "value", &jsonValue))
									{

										// get the channel number
										channelCmd.value = json_object_get_int(jsonValue);
										kbus_write(mosq, controller, channelCmd.module, channelCmd.channel, channelCmd.value);
									}
									if (json_object_object_get_ex(jsonChannel, "label", &jsonLabel))
									{

										// ********** TODO ***************
									}
									break;
								}
							}
						}
						else if (json_object_object_get_ex(jsonChannel, "label", &jsonLabel))
						{

						}
					}
				}
			}
		}
	}
}

// free the owners
free(chn);
free(mod);
while (json_object_put(jsonLabel))
{
};
while (json_object_put(jsonValue))
{
};
while (json_object_put(jsonChannel))
{
};
while (json_object_put(jsonChannels))
{
};
while (json_object_put(jsonModule))
{
};
while (json_object_put(jsonModules))
{
};
while (json_object_put(jsonController))
{
};
while (json_object_put(jsonDesired))
{
};
while (json_object_put(jsonState))
{
};
while (json_object_put(parsed_json))
{
};
/*json_object_put(jsonLabel);
json_object_put(jsonValue);
json_object_put(jsonChannel);
json_object_put(jsonChannels);
json_object_put(jsonModule);
json_object_put(jsonModules);
json_object_put(jsonController);
json_object_put(jsonDesired);
json_object_put(jsonState);
json_object_put(parsed_json);*/

// return error
return 0;
}

char *build_error_object(bool error, struct node controller, struct prog_config this_config, char *error_msg)
{
	struct json_object *json_error_object = json_object_new_object();
	json_object_object_add(json_error_object, "node_id", json_object_new_string(this_config.node_id));
	json_object_object_add(json_error_object, "switch_state", json_object_new_string(controller.switch_state));
	json_object_object_add(json_error_object, "error", json_object_new_boolean(error));
	json_object_object_add(json_error_object, "error_msg", json_object_new_string(error_msg));
	char *return_string = json_object_to_json_string(json_error_object);
	return return_string;
}

void json_parse_assign_label(int iModule, int iChannel)
{
	switch (controller.modules)
}