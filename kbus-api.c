//Copyright <2022> <Jesse Cox>
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this 
//software and associated documentation files (the "Software"), to deal in the Software 
//without restriction, including without limitation the rights to use, copy, modify, 
//merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
//permit persons to whom the Software is furnished to do so, subject to the following:

//The above copyright notice and this permission notice shall be included in all copies
//or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
//INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
//PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
//HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
//CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
//OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

//--------------------------------------------------------------------------
///	\file		kbus-api
///
///
///	\version	0.0.4 (beta)
///
///
///	\brief		open source project to interact with the kbus process image via mqtt
///
///
///	\author		Jesse Cox jesse.cox@wago.com
///
//--------------------------------------------------------------------------

#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

#include <mosquitto.h>
#include "kbus-api.h"
#include "get_config.h"
#include "mqtt.h"
#include "led.h"

#define IS_RUNNING	0x01
#define IS_STOPPED	0x02
#define IS_ERROR	0x80

struct node controller;

static int run = 1;
int switch_state = 0;
int initialized = 0;
int kbusIsInit = 1;

int iCounts = 0;

void selector_switch()
{
	switch_state = get_switch_state();
}

int main(int argc, char *argv[])
{
	uint8_t reconnect = true;
	struct mosquitto *mosq;
	int rc = 0;
	
	int kbus_resp = 0;

	// get the config
	this_config = get_program_config();

	controller.nodeId = this_config.node_id;
	
	set_led(IS_STOPPED);

	while (1)
	{
		// get the run stop reset switch value
		selector_switch();
		
		controller.switch_state = map_switch_state(switch_state);

		// run based on selector switch
		switch (switch_state)
		{
			// stopped
		case 0:
			if (initialized)
			{
				// send one last object to notify the system that it is stopped
				build_controller_object(mosq);
				
				// set the led to stop
				set_led(IS_STOPPED);

				printf("program stopped\n");
				// disconnect from the broker 
				mosquitto_disconnect(mosq);

				// reset the mqtt objects
				mosquitto_destroy(mosq);

				// decrement the lib ref
				mosquitto_lib_cleanup();

				// reset the init
				initialized = 0;
			}
			break;

			// run
		case 1:

			if (initialized == 1)
			{
				
				rc = mosquitto_loop(mosq, -1, 1);
				if (run && rc)
				{
					set_led(IS_ERROR);
					initialized = 0;
					printf("connection error!\n");
					sleep(3);
					mosquitto_reconnect(mosq);
				}
				
				if (iCounts >= 20)
				{
					build_controller_object(mosq);
					iCounts = 0;
				}
				else
				{
					iCounts++;
				}
				
				// do the kbus work
				kbus_resp = kbus_read(&mosq, &this_config, &kbus);//, controller);
				
				// send some error stuff if needed
				if (kbus_resp != 0) 
				{
					char *kbus_error_string = build_error_object(true, controller, this_config, "kbus error present");
					mosquitto_publish(mosq, NULL, this_config.status_pub_topic, strlen(kbus_error_string), kbus_error_string, 0, 0);
				}
			}
			else
			{
				set_led(IS_STOPPED);
				
				mosquitto_lib_init();

				mosq = mosquitto_new(controller.nodeId, true, 0);

				// if tls enabled add tls object to mosq
				if (this_config.support_tls)
				{
					mosquitto_tls_set(mosq, this_config.rootca_path, NULL, this_config.cert_path, this_config.key_path, NULL);
				}
				// if tls enabled add tls object to mosq
				if (this_config.support_userpasswd)
				{
					mosquitto_username_pw_set(mosq, this_config.mqtt_username, this_config.mqtt_password);
				}

				if (mosq)
				{
					mosquitto_connect_callback_set(mosq, connect_callback);
					mosquitto_message_callback_set(mosq, message_callback);

					rc = mosquitto_connect(mosq, this_config.mqtt_endpoint, this_config.mqtt_port, 0);

					mosquitto_subscribe(mosq, NULL, this_config.event_sub_topic, 0);

					if (kbusIsInit)
					{
						// scan the kbus
						kbus_init(&kbus);
						kbusIsInit = 0;						
					}

					// map everything to the controller object as part of the init process
					if (build_module_object(kbus.terminalCount, kbus.terminalDescription, kbus.terminals, &controller.modules))
					{
						controller.number_of_modules = kbus.terminalCount;
					}
				}
				build_controller_object(mosq);
				initialized = 1;
				set_led(IS_RUNNING);

			}
			break;
			// reset
		case 3:
			printf("reset tripped\n");
			
			//setRunLEDColor(RUN_COLOR_BLINK);

			// put some cool reset logic here
			
			initialized = 0;
			break;
		}
	}
	return rc;
}