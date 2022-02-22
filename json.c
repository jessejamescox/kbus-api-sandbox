//  This file is part of kbus_mqtt_client.
//--------------------------------------------------------------------------
//  kbus_mqtt_client is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//--------------------------------------------------------------------------
//  kbus_mqtt_client is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//--------------------------------------------------------------------------
//  You should have received a copy of the GNU General Public License
//  along with kbus_mqtt_client.  If not, see <https://www.gnu.org/licenses/>.
//--------------------------------------------------------------------------
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
#include <time.h>

#define MAX_IMG_LENGTH 2048

struct prog_config this_config;

// helper functions
struct json_object *dig_channel_object(struct channel channel)
{
	struct json_object *tmp = json_object_new_object();
	
	json_object_object_add(tmp, "value", json_object_new_boolean(channel.value)); 
	json_object_object_add(tmp, "label", json_object_new_string(channel.label)); 
	printf("Channel label: %s\n", channel.label);
	return tmp;
};

struct json_object *analog_channel_object(struct channel channel)
{
	struct json_object *tmp = json_object_new_object();
	
	json_object_object_add(tmp, "value", json_object_new_boolean(channel.value)); 
	json_object_object_add(tmp, "deadband", json_object_new_int(channel.deadband));
	json_object_object_add(tmp, "label", json_object_new_string(channel.label)); 
	return tmp;
};

struct json_object *simple_channels_object(struct module module)
{
	struct json_object *tmp = json_object_new_object();
	
	// add the channels
	for (int channelIndex = 0; channelIndex < module.channelCount; channelIndex++) 
	{
		// build the channel object key
		char *chn = (char *) malloc(10);
		asprintf(&chn, "channel%i", (channelIndex + 1));
		
		// check for digital
		if ((!strcmp(module.type, "DI")) || (!strcmp(module.type, "DO")))
		{
			json_object_object_add(tmp, chn, dig_channel_object(module.channel[channelIndex]));
		}
		
		// check for typical analog
		if ((!strcmp(module.type, "AI")) || (!strcmp(module.type, "AO")))
		{
			json_object_object_add(tmp, chn, analog_channel_object(module.channel[channelIndex]));
		}
		
		// free the holding char
		free(chn);
	}
	return tmp;
};

struct json_object *simple_module_object(struct module module)
{
	struct json_object *tmp = json_object_new_object();
	
	// add the module info
	json_object_object_add(tmp, "pn", json_object_new_int(module.pn));
	json_object_object_add(tmp, "position", json_object_new_int(module.position));
	json_object_object_add(tmp, "type", json_object_new_string(module.type));
	json_object_object_add(tmp, "input_channel_count", json_object_new_int(module.inChannelCount));
	json_object_object_add(tmp, "output_channel_count", json_object_new_int(module.outChannelCount));
	
	// add the channel info
	json_object_object_add(tmp, "channels", simple_channels_object(module));
	
	return tmp ;
}

struct json_object *simple_modules_object(struct node controller)
{
	struct json_object *tmp = json_object_new_object();
	
	// add the channels
	for(int moduleIndex = 0 ; moduleIndex < controller.number_of_modules ; moduleIndex++) 
{
	// build the channel object key
	char *mod = (char *) malloc(10);
	asprintf(&mod, "module%i", (moduleIndex + 1)) ;
		
	// build the objects in a loop
	json_object_object_add(tmp, mod, simple_module_object(controller.modules[moduleIndex])) ;
		
	// free the holding char
	free(mod) ;
}
return tmp ;
}

struct json_object *main_controller_object(struct node controller)
{
	struct json_object *tmp = json_object_new_object();
	
	// add the module info
	json_object_object_add(tmp, "node_id", json_object_new_string(controller.nodeId)) ;
	json_object_object_add(tmp, "switch_state", json_object_new_string(controller.switch_state));
	json_object_object_add(tmp, "module_count", json_object_new_int(controller.number_of_modules)) ;
	
	// add the channel info
	json_object_object_add(tmp, "modules", simple_modules_object(controller)) ;
	
	//char *tempString = json_object_to_json_string_ext(tmp, JSON_C_TO_STRING_PRETTY);
	//printf("%s\nis %i bytes\n", tempString, strlen(tempString));
	
	return tmp ;
}

char *main_message_object(struct node controller)
{
	
	struct json_object *tmp = json_object_new_object();
	
	// no idea if this will work
	//json_object_object_add(tmp, "state", tmp);
	//json_object_object_add(tmp, "reported", tmp);
	json_object_object_add(tmp, "controller", main_controller_object(controller));

	return tmp;
	//char *tempString = json_object_to_json_string_ext(tmp, JSON_C_TO_STRING_PRETTY);
	//printf("%s\nis %i bytes\n", tempString, strlen(tempString));
}

int build_controller_object(struct mosquitto *mosq, struct node controller) 
{ 
	struct json_object *tmp = main_message_object(controller);
	
	char *jsonString = json_object_to_json_string_ext(tmp, JSON_C_TO_STRING_PRETTY);
	printf("%s\n", jsonString);
}

/*
int build_controller_object(struct mosquitto *mosq, struct node controller) {
	
	//index vars
	int iModules, iChannels;
	
	int myJsonLen = 0;
	int myJsonPuts = 0;
	
	// main object creation
	struct json_object	*jsonMain		= json_object_new_object();
	struct json_object	*jsonState		= json_object_new_object();
	struct json_object	*jsonReported	= json_object_new_object();
	struct json_object	*jsonController	= json_object_new_object();
	struct json_object	*jsonModules	= json_object_new_object();
	//struct json_object	*jsonChannel	= json_object_new_object();
	//struct json_object	*jsonModule		= json_object_new_object();
	//struct json_object	*jsonChannels	= json_object_new_object();
	
	for (iModules = 0; iModules < controller.number_of_modules; iModules++) {
		
		struct json_object	*jsonModule		= json_object_new_object();
		struct json_object	*jsonChannels	= json_object_new_object();
		
		// build the module object one-by-one
		char *mod = (char *) malloc(10);
		asprintf(&mod, "module%i", (iModules + 1));
		
		for (iChannels = 0; iChannels < controller.modules[iModules].channelCount; iChannels++) {
			
			struct json_object	*jsonChannel = json_object_new_object();
			
			if (!strcmp(controller.modules[iModules].type, "DI"))
			{
				json_object_object_add(jsonChannel, "value", json_object_new_boolean(controller.modules[iModules].channelData[iChannels]));
			}
			if (!strcmp(controller.modules[iModules].type, "DO"))
			{
				json_object_object_add(jsonChannel, "value", json_object_new_boolean(controller.modules[iModules].channelData[iChannels]));
			}
			if (!strcmp(controller.modules[iModules].type, "AI"))
			{
				json_object_object_add(jsonChannel, "value", json_object_new_int(controller.modules[iModules].channelData[iChannels]));
			}
			if (!strcmp(controller.modules[iModules].type, "AO"))
			{
				json_object_object_add(jsonChannel, "value", json_object_new_int(controller.modules[iModules].channelData[iChannels]));
			}
			
			// build the modules channels one-by-one
			char *chn = (char *) malloc(10);
			asprintf(&chn, "channel%i", (iChannels + 1));
			
			// build the module object
			json_object_object_add(jsonChannels, chn, json_object_get(jsonChannel));
			
			// ***** do I need to free this?? 
			myJsonLen = json_object_object_length(jsonChannel);
			while (json_object_put(jsonChannel) != 0) 
			{
				json_object_put(jsonChannel);
			};
			myJsonLen = json_object_object_length(jsonChannel);
			free(chn);
			
		}
		
		json_object_object_add(jsonModule, "pn", json_object_new_int(controller.modules[iModules].pn));
		json_object_object_add(jsonModule, "module_type", json_object_new_string(controller.modules[iModules].type));
		json_object_object_add(jsonModule, "position", json_object_new_int(iModules + 1));
		json_object_object_add(jsonModule, "channel_count", json_object_new_int(controller.modules[iModules].channelCount));
		json_object_object_add(jsonModule, "channels", json_object_get(jsonChannels));
		
		myJsonLen = json_object_object_length(jsonChannels);
		json_object_put(jsonChannels);
		
		// add this to the reported object
		json_object_object_add(jsonModules, mod, json_object_get(jsonModule));
		
		myJsonLen = json_object_object_length(jsonModule);
		while (json_object_put(jsonModule) != 0)
		{
			myJsonPuts = json_object_put(jsonModule);
		}
		myJsonLen = json_object_object_length(jsonModule);
		free(mod);
		
	}
	
	// build the main json object	
	json_object_object_add(jsonController, "node_id", json_object_new_string(controller.nodeId));
	json_object_object_add(jsonController, "switch_state", json_object_new_int(controller.switch_state));
	json_object_object_add(jsonController, "module_count", json_object_new_int(controller.number_of_modules));
	
	// add this to the reported object
	json_object_object_add(jsonController, "modules", json_object_get(jsonModules));
	//json_object_put(jsonModules);
	
	// add this to the reported object
	json_object_object_add(jsonReported, "controller", json_object_get(jsonController));
	//json_object_put(jsonController);
	
	// add the module to the reported
	json_object_object_add(jsonState, "reported", json_object_get(jsonReported));
	//json_object_put(jsonReported);
	
	// add the reported to the state
	json_object_object_add(jsonMain, "state", json_object_get(jsonState));
	//json_object_put(jsonState);
	
	char *controller_string = json_object_to_json_string(jsonMain);
	
	
	int pub_resp = mosquitto_publish(mosq, NULL, this_config.status_pub_topic, strlen(controller_string), controller_string, 0, 0);
	
	
	//json_object_put(jsonChannel);
	//json_object_put(jsonChannels);
	//json_object_put(jsonModule);
	json_object_put(jsonModules);
	json_object_put(jsonController);
	json_object_put(jsonReported);
	json_object_put(jsonState);
	json_object_put(jsonMain);
	
	return 0;
}
*/

void build_event_object(struct mosquitto *mosq, struct node controller, int modulePosition, int channelPosition, int channelValue) {
	
	char *ch = (char *) malloc(10);
	char *md = (char *) malloc(10);
	
	asprintf(&md, "module%i", (modulePosition + 1));
	asprintf(&ch, "channel%i", (channelPosition + 1));
	 
	struct json_object	*jsonMain = json_object_new_object();
	struct json_object	*jsonState = json_object_new_object();
	struct json_object	*jsonReported = json_object_new_object();
	struct json_object	*jsonController = json_object_new_object();
	struct json_object	*jsonModules = json_object_new_object();
	struct json_object	*jsonModule = json_object_new_object();
	struct json_object	*jsonChannels = json_object_new_object();
	struct json_object	*jsonChannel = json_object_new_object();
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
	json_object_object_add(jsonChannels, ch, json_object_get(jsonChannel));
	
	json_object_object_add(jsonModule, "channels", json_object_get(jsonChannels));
	
	
	// add this to the reported object
	json_object_object_add(jsonModules, md, json_object_get(jsonModule));
	
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
	
	free(md);
	free(ch);
	
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

int parse_mqtt(struct mosquitto *mosq, char *message) {
	
	struct channel_command channelCmd;
	
	struct json_object *jsonHold;
	
	int pjFree, jhFree, jh2Free;
	
	char *mod, *chn;

	// get the main json object
	struct json_object *parsed_json		= json_tokener_parse(message);
	
	mod = (char*)malloc(10 * sizeof(char));
	chn = (char*)malloc(10 * sizeof(char));
	
	// start checking the json objects
	if (json_object_object_get_ex(parsed_json, "state", &jsonHold)) {
		if (json_object_object_get_ex(jsonHold, "desired", &jsonHold)) {
			if (json_object_object_get_ex(jsonHold, "controller", &jsonHold)) {
				if (json_object_object_get_ex(jsonHold, "modules", &jsonHold)) {
					
					// still not sure why I have to do this
					struct json_object *jsonHold2 = jsonHold;
					
					for (int iModules = 0; iModules < controller.number_of_modules; iModules++) {

						asprintf(&mod, "module%i", (iModules + 1));
						
						// find the object 
						if (json_object_object_get_ex(jsonHold, mod, &jsonHold2)) {
							
							// found the channel, record the module position
							channelCmd.module = iModules;
							
							if (json_object_object_get_ex(jsonHold2, "channels", &jsonHold2))
							{
							
								// search for the channels
								for (int iChannels = 0; iChannels < controller.modules[iModules].channelCount; iChannels++)
								{
									asprintf(&chn, "channel%i", (iChannels + 1));
									
									// find the object 
									if (json_object_object_get_ex(jsonHold2, chn, &jsonHold2))
									{
										
										// found the channel, record the module position
										channelCmd.channel = iChannels;
										
										if (json_object_object_get_ex(jsonHold2, "value", &jsonHold2))
										{
											
											// get the channelk number
											channelCmd.value = json_object_get_int(jsonHold2);
											
											// write the kbus regs
											kbus_write(mosq, controller, channelCmd.module, channelCmd.channel, channelCmd.value);		
											
											jh2Free = json_object_put(jsonHold2);
											//printf("jh2Free: %d\n", jh2Free);
											jh2Free = json_object_put(jsonHold2);
											//printf("jh2Free: %d\n", jh2Free);
											break;
											
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// free the owners	
	
	pjFree = json_object_put(parsed_json);
	//printf("pjFree: %d\n", pjFree);
	pjFree = json_object_put(parsed_json);
	//printf("pjFree: %d\n", pjFree);
	
	jhFree = json_object_put(jsonHold);
	//printf("jhFree: %d\n", jhFree);
	
	free(chn);
	free(mod);
	
	//return error
	return 0;
}

char *build_error_object(bool error, struct node controller, struct prog_config this_config, char *error_msg) {
	struct json_object *json_error_object = json_object_new_object();
	json_object_object_add(json_error_object, "node_id", json_object_new_string(this_config.node_id));
	json_object_object_add(json_error_object, "switch_state", json_object_new_string(controller.switch_state));
	json_object_object_add(json_error_object, "error", json_object_new_boolean(error));
	json_object_object_add(json_error_object, "error_msg", json_object_new_string(error_msg));
	char *return_string = json_object_to_json_string(json_error_object);
	return return_string;
}