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
#include "node.h"
#include "logger.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

int build_module_object(int terminalCount, tldkc_KbusInfo_TerminalInfo terminalDescription[LDKC_KBUS_TERMINAL_COUNT_MAX], u16 terminals[LDKC_KBUS_TERMINAL_COUNT_MAX], struct module modules[LDKC_KBUS_TERMINAL_COUNT_MAX]) {

	// cycle through the modules and read the values
	for (int i = 0; i < terminalCount; i++) 
	{
		printf("Terminals: %d\n", terminals[i]);
		modules[i].position = i + 1;
		switch (terminals[i])
		{
		case 400 ... 499:
			modules[i].type = "AI";
			modules[i].pn = terminals[i];
			modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
			modules[i].inChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
			modules[i].outChannelCount = 0;
			modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
			log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
			break;
		case 500 ... 599:
			modules[i].type = "AO";
			modules[i].pn = terminals[i];
			modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
			modules[i].inChannelCount = 0;
			modules[i].outChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
			modules[i].bitOffsetOut = terminalDescription[i].OffsetOutput_bits;
			log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
			break;
		case 34000 ... 37000:			
			if ((CHECK_BIT(terminals[i], 0)) && (CHECK_BIT(terminals[i], 1))) {
				modules[i].type = "DX";
				modules[i].pn = terminals[i];
				modules[i].channelCount = ((terminalDescription[i].SizeInput_bits) + (terminalDescription[i].SizeOutput_bits));
				modules[i].inChannelCount = (terminalDescription[i].AdditionalInfo.ChannelCount);
				modules[i].outChannelCount = (terminalDescription[i].SizeOutput_bits);
				modules[i].bitOffsetOut = terminalDescription[i].OffsetOutput_bits;
				modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
				log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
				break;
			}
			if ((!CHECK_BIT(terminals[i], 0)) && (CHECK_BIT(terminals[i], 1))) {
				modules[i].type = "DO";
				modules[i].pn = terminals[i];
				modules[i].channelCount = (terminalDescription[i].SizeOutput_bits);
				modules[i].inChannelCount = 0;
				modules[i].outChannelCount = (terminalDescription[i].SizeOutput_bits);
				modules[i].bitOffsetOut = terminalDescription[i].OffsetOutput_bits;
				log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
				break;
			}
			if ((CHECK_BIT(terminals[i], 0)) && (!CHECK_BIT(terminals[i], 1))) {
				modules[i].type = "DI";
				modules[i].pn = terminals[i];
				modules[i].channelCount = (terminalDescription[i].SizeInput_bits);
				modules[i].inChannelCount = (terminalDescription[i].SizeInput_bits);
				modules[i].outChannelCount = 0;
				modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
				log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
				break;
			}
			// default to "unsupported module type	
		default :
			modules[i].type = "unsupported";
			modules[i].pn = terminals[i];
			log_error("Module %d is unsupported", i);
			break;
		}
		for (int y = 0; y < modules[i].channelCount; y++) 
		{
			// build the channel label key
			asprintf(&modules[i].channel[y].label, "m%ic%i", (i + 1), (y + 1));

			//printf("%s\n", modules[i].channel[y].label);

		}
	} 
	return 1;
}

char *map_switch_state(int switch_state) {
	switch (switch_state) {
	case -1:
		return "ERROR";
	case 0:
		return "STOP";
	case 1:
		return "RUN";
	case 8:
		return "RESET";
	case 128:
		return "RESET_ALL";
	default:
		return "NA";
	}
}
