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
};

struct json_object *analog_out_channel_object(int mp, int cp)
{
	struct json_object *jobj = json_object_new_object() ;
	json_object_object_add(jobj, "value", json_object_new_int(aoMod[controller.modules[mp].typeIndex].outData[cp].value)) ;
	json_object_object_add(jobj, "label", json_object_new_string(aoMod[controller.modules[mp].typeIndex].outData[cp].label)) ;
	return jobj ;
}

struct json_object *pmm_module_object(int mp)
{
	struct json_object *jobj = json_object_new_object() ;
	struct json_object *jsL1 = json_object_new_object() ;
	struct json_object *jsL2 = json_object_new_object() ;
	struct json_object *jsL3 = json_object_new_object() ;

	json_object_object_add(jsL1, "voltage", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L1.measuredVolts * 0.01) * 100) / 100)) ;
	json_object_object_add(jsL1, "current", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L1.measuredAmps * 0.0001) * 100) / 100)) ;
	json_object_object_add(jsL1, "power", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L1.measuredPower * 0.01) * 100) / 100)) ;
	json_object_object_add(jsL1, "frequency", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L1.measuredFrequency * 0.001) * 100) / 100)) ;

	json_object_object_add(jsL1, "errorGeneral", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L1.errorGeneral)) ;

	/*json_object_object_add(jsL1, "errorUnderVolts", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L1.errorUnderVolts)) ;
	json_object_object_add(jsL1, "errorOverVolts", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L1.errorOverVolts)) ;
	json_object_object_add(jsL1, "errorCurrentRange", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L1.errorCurrentRange)) ;
	json_object_object_add(jsL1, "errorOverCurrent", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L1.errorOverCurrent)) ;
	json_object_object_add(jsL1, "errorVoltsRange", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L1.errorVoltsRange)) ;
	json_object_object_add(jsL1, "errorFieldCCW", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L1.errorFieldCCW)) ;
	json_object_object_add(jsL1, "errorZeroCross", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L1.errorZeroCross)) ;*/

	json_object_object_add(jsL2, "voltage", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L2.measuredVolts * 0.01) * 100) / 100)) ;
	json_object_object_add(jsL2, "current", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L2.measuredAmps * 0.0001) * 100) / 100)) ;
	json_object_object_add(jsL2, "power", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L2.measuredPower * 0.01) * 100) / 100)) ;
	json_object_object_add(jsL2, "frequency", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L2.measuredFrequency * 0.001) * 100) / 100)) ;

	json_object_object_add(jsL2, "errorGeneral", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L2.errorGeneral)) ;

	/*json_object_object_add(jsL2, "errorUnderVolts", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L2.errorUnderVolts)) ;
	json_object_object_add(jsL2, "errorOverVolts", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L2.errorOverVolts)) ;
	json_object_object_add(jsL2, "errorCurrentRange", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L2.errorCurrentRange)) ;
	json_object_object_add(jsL2, "errorOverCurrent", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L2.errorOverCurrent)) ;
	json_object_object_add(jsL2, "errorVoltsRange", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L2.errorVoltsRange)) ;
	json_object_object_add(jsL2, "errorFieldCCW", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L2.errorFieldCCW)) ;
	json_object_object_add(jsL2, "errorZeroCross", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L2.errorZeroCross)) ;*/

	json_object_object_add(jsL3, "voltage", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L3.measuredVolts * 0.01) * 100) / 100)) ;
	json_object_object_add(jsL3, "current", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L3.measuredAmps * 0.0001) * 100) / 100)) ;
	json_object_object_add(jsL3, "power", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L3.measuredPower * 0.01) * 100) / 100)) ;
	json_object_object_add(jsL3, "frequency", json_object_new_double(round((pmMod[controller.modules[mp].typeIndex].L3.measuredFrequency * 0.001) * 100) / 100)) ;

	json_object_object_add(jsL3, "errorGeneral", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L3.errorGeneral)) ;

	/*json_object_object_add(jsL3, "errorUnderVolts", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L3.errorUnderVolts)) ;
	json_object_object_add(jsL3, "errorOverVolts", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L3.errorOverVolts)) ;
	json_object_object_add(jsL3, "errorCurrentRange", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L3.errorCurrentRange)) ;
	json_object_object_add(jsL3, "errorOverCurrent", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L3.errorOverCurrent)) ;
	json_object_object_add(jsL3, "errorVoltsRange", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L3.errorVoltsRange)) ;
	json_object_object_add(jsL3, "errorFieldCCW", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L3.errorFieldCCW)) ;
	json_object_object_add(jsL3, "errorZeroCross", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].L3.errorZeroCross)) ;*/

	json_object_object_add(jobj, "L1", json_object_get(jsL1)) ;
	json_object_object_add(jobj, "L2", json_object_get(jsL2)) ;
	json_object_object_add(jobj, "L3", json_object_get(jsL3)) ;
	json_object_object_add(jobj, "errorGroup", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].grpError)) ;
	json_object_object_add(jobj, "errorGeneral", json_object_new_boolean(pmMod[controller.modules[mp].typeIndex].genError)) ;

	json_object_put(jsL1) ;
	json_object_put(jsL2) ;
	json_object_put(jsL3) ;

	return jobj ;
}

struct json_object *din_data_breakout_object(int mp)
{
	struct json_object *jobj = json_object_new_object() ;

	for(int channelIndex = 0 ; channelIndex < controller.modules[mp].inChannelCount ; channelIndex++)
	{
		char *chn = (char *)malloc(10 * sizeof(char)) ;
		strcpy(chn, "channel") ;
		char *ch ;
		itoa((channelIndex + 1), &ch, 10) ;
		strcat(chn, &ch) ;

		json_object_object_add(jobj, chn, dig_in_channel_object(mp, channelIndex)) ;

		free(chn) ;
	}
	return jobj ;
}

struct json_object *dout_data_breakout_object(int mp)
{
	struct json_object *jobj = json_object_new_object();

	for (int channelIndex = 0; channelIndex < controller.modules[mp].outChannelCount; channelIndex++)
	{
		char *chn = (char *)malloc(10 * sizeof(char));
		strcpy(chn, "channel");
		char *ch;
		itoa((channelIndex + 1), &ch, 10);
		strcat(chn, &ch);

		json_object_object_add(jobj, chn, dig_out_channel_object(mp, channelIndex));

		free(chn);
	}
	return jobj;
}

struct json_object *dxi_data_breakout_object(int mp)
{
	struct json_object *jobj = json_object_new_object();

	for (int channelIndex = 0; channelIndex < controller.modules[mp].inChannelCount; channelIndex++)
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

struct json_object *dxo_data_breakout_object(int mp)
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

struct json_object *ain_data_breakout_object(int mp)
{
	struct json_object *jobj = json_object_new_object();

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
return jobj;
}

struct json_object *aout_data_breakout_object(int mp)
{
	struct json_object *jobj = json_object_new_object();

	for (int channelIndex = 0; channelIndex < controller.modules[mp].outChannelCount; channelIndex++)
	{
		char *chn = (char *)malloc(10 * sizeof(char));
		strcpy(chn, "channel");
		char *ch;
		itoa((channelIndex + 1), &ch, 10);
		strcat(chn, &ch);

		json_object_object_add(jobj, chn, analog_out_channel_object(mp, channelIndex));

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
		json_object_object_add(jobj, "inputs", din_data_breakout_object(mp));
		break;

	case dom:
		json_object_object_add(jobj, "outputs", dout_data_breakout_object(mp));
		break;

	case dxm:
		//json_object_object_add(jobj, "inputs", dxi_data_breakout_object(mp));
		json_object_object_add(jobj, "outputs", dxo_data_breakout_object(mp));
		json_object_object_add(jobj, "inputs", dxi_data_breakout_object(mp));
		break;

	case aim:
		json_object_object_add(jobj, "inputs", ain_data_breakout_object(mp));
		break;

	case aom:
		json_object_object_add(jobj, "outputs", aout_data_breakout_object(mp));
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
			json_object_object_add(jobj, "process_data", pmm_module_object(mp));
			break;
		}
	}
	else // simple modules
	{
		json_object_object_add(jobj, "input_channel_count", json_object_new_int(controller.modules[mp].inChannelCount));
		json_object_object_add(jobj, "output_channel_count", json_object_new_int(controller.modules[mp].outChannelCount));

		// add the channel info
		json_object_object_add(jobj, "process_data", simple_channels_object(mp));
	}

	return jobj;
};

struct json_object *simple_modules_object()
{
	struct json_object *jobj = json_object_new_object() ;

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
	struct json_object *jobj = json_object_new_object() ;

	// add the module info
	json_object_object_add(jobj, "node_id", json_object_new_string(controller.nodeId));
	json_object_object_add(jobj, "timestamp", json_object_new_int((int)time(NULL)));
	json_object_object_add(jobj, "switch_state", json_object_new_string(controller.switch_state));
	json_object_object_add(jobj, "module_count", json_object_new_int(controller.number_of_modules));

	// add the channel info
	json_object_object_add(jobj, "modules", simple_modules_object());

	return jobj ;
}

void build_controller_object(struct mosquitto *mosq)
{
	struct json_object *jsState = json_object_new_object();
	struct json_object *jsReported = json_object_new_object();
	json_object_object_add(jsReported, "reported", main_controller_object());
	json_object_object_add(jsState, "state", jsReported);

	char *jsonString = json_object_to_json_string(jsState);

	int msq_pub = mosquitto_publish(mosq, NULL, this_config.status_pub_topic, strlen(jsonString), jsonString, 0, 0);

	// while (json_object_put(jsController)) {};
	while(json_object_put(jsReported)){};
	while(json_object_put(jsState)){} ;
}

void parse_input_channel_data(struct mosquitto *mosq, struct json_object *jObj, int iModules)
{
	struct json_object *jLbl, *jDb;
	char *chn;
	chn = (char *)malloc(10 * sizeof(char));
	
	for (int iChannels = 0; iChannels < controller.modules[iModules].channelCount; iChannels++)
	{
		
		strcpy(chn, "channel");
		char *ch;
		itoa((iChannels + 1), &ch, 10);
		strcat(chn, &ch);
		
		struct json_object *jH = jObj;

		// find the object
		if (json_object_object_get_ex(jH, chn, &jH))
		{
			if (controller.modules[iModules].mtype == aim)
			{
				if (json_object_object_get_ex(jH, "deadband", &jDb)) 
				{
					aiMod[controller.modules[iModules].typeIndex].inData[iChannels].deadband = json_object_get_int(jDb);
				}
				
				if (json_object_object_get_ex(jH, "label", &jLbl)) 
				{
					char *label = json_object_get_string(jLbl);
					memcpy(aiMod[controller.modules[iModules].typeIndex].inData[iChannels].label, label, strlen(label) + 1);
				}
				break;
			}
			
			if ((controller.modules[iModules].mtype == dim) | (controller.modules[iModules].mtype == dxm))
			{
				
				if (json_object_object_get_ex(jH, "label", &jLbl)) 
				{
					char *label = json_object_get_string(jLbl);
					memcpy(diMod[controller.modules[iModules].typeIndex].inData[iChannels].label, label, strlen(label) + 1);
				}
				break;
			}
		}
		json_object_put(jH);
	}
	free(chn);
	json_object_put(jLbl);
	json_object_put(jDb);
	json_object_put(jObj);
	
	return;
}

void parse_output_channel_data(struct mosquitto *mosq, struct json_object *jObj, int iModules)
{
	int channelCommand;
	struct json_object *jLbl, *jVal;
	char *chn = (char *)malloc(10 * sizeof(char));
	
	for (int iChannels = 0; iChannels < controller.modules[iModules].channelCount; iChannels++)
	{
		strcpy(chn, "channel");
		char *ch;
		itoa((iChannels + 1), &ch, 10);
		strcat(chn, &ch);
		
		struct json_object *jH = jObj;

		// find the object
		if (json_object_object_get_ex(jH, chn, &jH))
		{
			if (controller.modules[iModules].mtype == aom)
			{
				struct json_object *jDb;
				if (json_object_object_get_ex(jH, "deadband", &jDb)) 
				{
					aoMod[controller.modules[iModules].typeIndex].outData[iChannels].deadband = json_object_get_int(jDb);
				}
				
				if (json_object_object_get_ex(jH, "label", &jLbl)) 
				{
					char *label = json_object_get_string(jLbl);
					memcpy(aoMod[controller.modules[iModules].typeIndex].outData[iChannels].label, label, strlen(label) + 1);
				}
				
				if (json_object_object_get_ex(jH, "value", &jVal)) 
				{
					channelCommand = json_object_get_int(jVal);
					kbus_write(mosq, controller, iModules, iChannels, channelCommand);
				}
				
				json_object_put(jDb);
			}
			
			if (controller.modules[iModules].mtype == dom)
			{
				if (json_object_object_get_ex(jH, "label", &jLbl)) 
				{
					char *label = json_object_get_string(jLbl);
					memcpy(doMod[controller.modules[iModules].typeIndex].outData[iChannels].label, label, strlen(label) + 1);
				}
				
				if (json_object_object_get_ex(jH, "value", &jVal)) 
				{
					channelCommand = json_object_get_int(jVal);
					kbus_write(mosq, controller, iModules, iChannels, channelCommand);
				}
			}
			
			if (controller.modules[iModules].mtype == dxm)
			{
				if (json_object_object_get_ex(jH, "label", &jLbl)) 
				{
					//char *label = json_object_get_string(jLbl);
					//memcpy(dxMod[controller.modules[iModules].typeIndex].outData[iChannels].label, label, strlen(label) + 1);
				}
				
				if (json_object_object_get_ex(jH, "value", &jVal)) 
				{
					channelCommand = json_object_get_int(jVal);
					kbus_write(mosq, controller, iModules, iChannels, channelCommand);
				}
			}
			break;
		}
		json_object_put(jH);
	}
	
	// free all the things
	free(chn);
	json_object_put(jVal);
	json_object_put(jLbl);
	json_object_put(jObj);
	
	return;
}


void parse_process_data(struct mosquitto *mosq, struct json_object *jObj, int iModules)
{
	struct json_object *jIn = jObj;
	struct json_object *jOut = jObj;
	// now the fun task of treating each module different
	switch (controller.modules[iModules].mtype)
	{
		// inputs only
	case dim:
	case aim:
		if (json_object_object_get_ex(jObj, "inputs", &jObj))	
		{
			parse_input_channel_data(mosq, jObj, iModules);
		}
		break;
							
		// outputs only
	case dom:
	case aom:
		if (json_object_object_get_ex(jObj, "outputs", &jObj))	
		{
			parse_output_channel_data(mosq, jObj, iModules);
		}
		break;
							
		// in / out module	
	case dxm:
		if (json_object_object_get_ex(jIn, "inputs", &jObj))	
		{
			parse_input_channel_data(mosq, jObj, iModules);
		} 
		if (json_object_object_get_ex(jOut, "outputs", &jObj))	
		{
			parse_output_channel_data(mosq, jObj, iModules);
		}
		break;
							
		// specialty
	case spm:
		// going to deal with this later
		break;
							
	default:
		break;
	}
	json_object_put(jIn);
	json_object_put(jOut);
}

void parse_module(struct mosquitto *mosq, struct json_object *jObj)
{ 
	char *mod = (char *)malloc(10 * sizeof(char));
	
	for (int iModules = 0; iModules < controller.number_of_modules; iModules++)
	{
		strcpy(mod, "module");
		char *mi;
		itoa((iModules + 1), &mi, 10);
		strcat(mod, &mi);
		
		struct json_object *jH = jObj;
					
		// find the object
		if (json_object_object_get_ex(jH, mod, &jH))
		{
			if (json_object_object_get_ex(jH, "process_data", &jH))	
			{
				parse_process_data(mosq, jH, iModules);
			}
		}
		json_object_put(jH);
	}
	free(mod);
	return;
}

void parse_mqtt(struct mosquitto *mosq, char *message)
{ 
	// step through from the main command
	struct json_object *parsed_json = json_tokener_parse(message);
	struct json_object *jObj;
	
	if (json_object_object_get_ex(parsed_json, "state", &jObj))
	{
		if (json_object_object_get_ex(jObj, "desired", &jObj))
		{
			if (json_object_object_get_ex(jObj, "modules", &jObj))
			{
				parse_module(mosq, jObj);			
			}
		}
	}
	json_object_put(jObj);
	json_object_put(parsed_json);
	
	return;
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