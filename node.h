#pragma once
#ifndef __NODE_H__
#define __NODE_H__

#include <dal/adi_application_interface.h>
#include <ldkc_kbus_information.h>
#include <ldkc_kbus_register_communication.h>
#include "logger.h"
#include "switch.h"
struct channel
{
	bool value;
	bool error;
	char *label;
	int deadband;
};

struct achannel	
{
	int value;
	bool error;
	int deadband;
	char *label;	
};

struct dchannel
{
	bool value;
	bool error;
	char *label;
};

struct	module 
{
	char *type;
	char *desc;
	uint8_t typeIndex;
	int pn;
	uint8_t position;
	uint8_t channelCount;
	uint8_t inChannelCount;
	uint8_t outChannelCount;
	uint16_t bitOffsetIn;
	uint16_t bitOffsetOut;
	struct channel channel[16];
	
};

struct node
{
	char *nodeId;
	char *switch_state;
	int ss;
	int	number_of_modules;	
	struct module modules[LDKC_KBUS_TERMINAL_COUNT_MAX];
};

struct diMod
{
	struct dchannel inData[16]; 
};

struct doMod
{
	struct dchannel outData[16]; 
};

struct dxMod
{
	struct dchannel inData[8]; 
	struct dchannel outData[8];
};

struct aiMod
{
	struct achannel inData[8]; 
};

struct aoMod
{
	struct achannel outData[8]; 
};

// structs and sub-structs of power measurement module
//sub structure for PMM
struct lX
{
	float volts;
	float ampres;
	float power;
	float frequency;
	bool error;
};

// main structure for PMM 
struct pmMod
{
	char *label;
	bool genError;
	bool grpError;
	
	struct lX L1;
	struct lX L2;
	struct lX L3;
};

extern int build_module_object(int terminalCount, tldkc_KbusInfo_TerminalInfo terminalDescription[LDKC_KBUS_TERMINAL_COUNT_MAX], u16 terminals[LDKC_KBUS_TERMINAL_COUNT_MAX], struct module modules[LDKC_KBUS_TERMINAL_COUNT_MAX]);

extern char *map_switch_state(int *switch_state);

#endif /*__NODE_H__*/