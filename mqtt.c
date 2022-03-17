//  This file is part of kbus-daemon.
//--------------------------------------------------------------------------
//  kbus-daemon is free software: you can redistribute it and/or modify
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

#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "kbus-api.h"
#include "get_config.h"
#include "json.h"

struct prog_config this_config;

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
	if (!(strcmp(message->topic, this_config.event_sub_topic))) {
		parse_mqtt(mosq, message->payload);
	}
}

void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
	mosquitto_subscribe(mosq, NULL, this_config.event_sub_topic, 0);
	log_execution("MQTT Broker Connection Success", 0);
}

void disconnect_callback(struct mosquitto *mosq, void *obj, int result)
{
	log_execution("MQTT Broker Connection Disconnected", 0);
}