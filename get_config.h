#pragma once
#ifndef __GET_CONFIG_H__
#define __GET_CONFIG_H__

#include <stdbool.h>
#include <libconfig.h>

struct	prog_config {
	char *node_id;
	bool publish_cyclic;
	int publish_cycle;
	char *mqtt_endpoint;
	int mqtt_port;
	bool support_userpasswd;
	char *mqtt_username;
	char *mqtt_password;
	bool support_tls;
	char *cert_path;
	char *key_path;
	char *rootca_path;
	bool support_aws_shadow;
	char *event_sub_topic;
	char *event_pub_topic;
	char *status_pub_topic;
	int analog_deadband;
};

extern struct prog_config this_config;

extern struct prog_config get_program_config();

#endif /*__GET_CONFIG_H__*/
#pragma once
