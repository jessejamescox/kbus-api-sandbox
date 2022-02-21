#ifndef __MQTT_H__
#define __MQTT_H__

#include <mosquitto.h>
#include "kbus-api.h"

extern void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);
extern void connect_callback(struct mosquitto *mosq, void *obj, int result);
#endif /*__MQTT_H__*/