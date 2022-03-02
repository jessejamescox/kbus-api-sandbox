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
#include "kbus.h"
#include "logger.h"
#include "utils.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

int build_module_object(int terminalCount, tldkc_KbusInfo_TerminalInfo terminalDescription[LDKC_KBUS_TERMINAL_COUNT_MAX], u16 terminals[LDKC_KBUS_TERMINAL_COUNT_MAX], struct module modules[LDKC_KBUS_TERMINAL_COUNT_MAX]) {
	
	// index vars for assigning the array positions
	uint8_t aiIndex, pmIndex, aoIndex, diIndex, doIndex, dxIndex = 0;

	terminals;
	// cycle through the modules and read the values
	for (int i = 0; i < terminalCount; i++) 
	{
		printf("Terminals: %d\n", terminals[i]);
		modules[i].position = i + 1;
		
		switch (terminals[i])
		{
		
			//*******************************************************************************************/
			//				ANALOG INPUTS
			//*******************************************************************************************/
			case 400 ... 499:
		
				// 0-20mA signal inputs
				if ((terminals[i] == 452) | 
					(terminals[i] == 453) |
					(terminals[i] == 465) |
					(terminals[i] == 470) |
					(terminals[i] == 472) |
					(terminals[i] == 480))
					{
						modules[i].type = "AI";
						modules[i].mtype = aim;
						modules[i].desc = "0-20mA analog input";
						modules[i].pn = terminals[i];
						modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].inChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].outChannelCount = 0;
						modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
						log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
			
						// map to type index
						modules[i].typeIndex = aiIndex;
						aiIndex++;
						break;
					}
		
				// 4-20mA signal inputs
				if ((terminals[i] == 454) | 
					(terminals[i] == 455) |
					(terminals[i] == 466) |
					(terminals[i] == 473) |
					(terminals[i] == 474) |
					(terminals[i] == 482) |
					(terminals[i] == 492))
					{

						modules[i].type = "AI";
						modules[i].mtype = aim;
						modules[i].desc = "4-20mA analog input";
						modules[i].pn = terminals[i];
						modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].inChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].outChannelCount = 0;
						modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
						log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
			
						// map to type index
						modules[i].typeIndex = aiIndex;
						aiIndex++;
						break;
					}
		
				// configurable 0/4-20mA analog input
				if (terminals[i] == 496)
					{
						modules[i].type = "AI";
						modules[i].mtype = aim;
						modules[i].desc = "0/4-20mA analog input ***warning: requires configuration";
						modules[i].pn = terminals[i];
						modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].inChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].outChannelCount = 0;
						modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
						log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
			
						// map to type index
						modules[i].typeIndex = aiIndex;
						aiIndex++;
						break;
					}
		
				// 1/5A analog input
				if (terminals[i] == 475)
					{
						modules[i].type = "AI";
						modules[i].mtype = aim;
						modules[i].desc = "1/5A analog input";
						modules[i].pn = terminals[i];
						modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].inChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].outChannelCount = 0;
						modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
						log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
			
						// map to type index
						modules[i].typeIndex = aiIndex;
						aiIndex++;
						break;
					}
		
				// +/-10vdc signal inputs
				if ((terminals[i] == 456) | 
					(terminals[i] == 457) |
					(terminals[i] == 476) |
					(terminals[i] == 479))
				{
						modules[i].type = "AI";
						modules[i].mtype = aim;
						modules[i].desc = "+/-10vdc analog input";
						modules[i].pn = terminals[i];
						modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].inChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].outChannelCount = 0;
						modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
						log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
			
						// map to type index
						modules[i].typeIndex = aiIndex;
						aiIndex++;
						break;
					}
		
				// 0-10vdc signal inputs
				if ((terminals[i] == 459) | 
					(terminals[i] == 467) |
					(terminals[i] == 468) |
					(terminals[i] == 477) |
					(terminals[i] == 478))
				{					
						modules[i].type = "AI";
						modules[i].mtype = aim;
						modules[i].desc = "0-10vdc analog input";
						modules[i].pn = terminals[i];
						modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].inChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].outChannelCount = 0;
						modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
						log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
			
						// map to type index
						modules[i].typeIndex = aiIndex;
						aiIndex++;
						break;
					}
		
				// configurable "0-10vdc / ±10vdc analog input 
				if (terminals[i] == 496)
					{
						modules[i].type = "AI";
						modules[i].mtype = aim;
						modules[i].desc = "0-10vdc / ±10vdc analog input ***warning: requires configuration";
						modules[i].pn = terminals[i];
						modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].inChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].outChannelCount = 0;
						modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
						log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
			
						// map to type index
						modules[i].typeIndex = aiIndex;
						aiIndex++;
						break;
					}
		
				// 0-30vdc analog input 
				if (terminals[i] == 483)
					{
						modules[i].type = "AI";
						modules[i].mtype = aim;
						modules[i].desc = "0-30vd analog input";
						modules[i].pn = terminals[i];
						modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].inChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].outChannelCount = 0;
						modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
						log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
			
						// map to type index
						modules[i].typeIndex = aiIndex;
						aiIndex++;
						break;
					}
		
				// resistance signal input
				if ((terminals[i] == 450) | 
					(terminals[i] == 451) |
					(terminals[i] == 461) |
					(terminals[i] == 463) |
					(terminals[i] == 464))
				{
						modules[i].type = "AI";
						modules[i].mtype = aim;
						modules[i].desc = "resistance sensor analog input";
						modules[i].pn = terminals[i];
						modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].inChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].outChannelCount = 0;
						modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
						log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
			
						// map to type index
						modules[i].typeIndex = aiIndex;
						aiIndex++;
						break;
					}
		
				// thermocouple signal inputs
				if ((terminals[i] == 458) | 
					(terminals[i] == 469) |
					(terminals[i] == 498))
				{
						modules[i].type = "AI";
						modules[i].mtype = aim;
						modules[i].desc = "thermocouple sensor analog input";
						modules[i].pn = terminals[i];
						modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].inChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].outChannelCount = 0;
						modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
						log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
			
						// map to type index
						modules[i].typeIndex = aiIndex;
						aiIndex++;
						break;
					}
		
				// resistor bridge / load cell / strain gauge signal inputs
				if ((terminals[i] == 491) | 
					(terminals[i] == 1491))
				{
						modules[i].type = "AI";
						modules[i].mtype = aim;
						modules[i].desc = "resistor bridge analog input";
						modules[i].pn = terminals[i];
						modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].inChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].outChannelCount = 0;
						modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
						log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
			
						// map to type index
						modules[i].typeIndex = aiIndex;
						aiIndex++;
						break;
					}
		
				// power measurement module
				if ((terminals[i] == 494) | 
					(terminals[i] == 495))
				{
						modules[i].type = "PMM";
						modules[i].mtype = spm;
						modules[i].desc = "Power Measurement Module";
						modules[i].pn = terminals[i];
						modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].inChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
						modules[i].outChannelCount = 0;
						modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
						modules[i].bitOffsetOut = terminalDescription[i].OffsetOutput_bits;
						log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
			
						// map to type index
						modules[i].typeIndex = pmIndex;
						pmIndex++;
						break;
					}

		//*******************************************************************************************/
		//				ANALOG OUTPUTS
		//*******************************************************************************************/
			
			case 500 ... 599:
			
				// 0-20mA signal outputs
				if ((terminals[i] == 552) | 
					(terminals[i] == 553))
				{
					modules[i].type = "AO";
					modules[i].mtype = aom;
					modules[i].desc = "0-20mA analog output";
					modules[i].pn = terminals[i];
					modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
					modules[i].inChannelCount = 0;
					modules[i].outChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
					modules[i].bitOffsetOut = terminalDescription[i].OffsetOutput_bits;
					log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetOut);
			
					// map to type index
					modules[i].typeIndex = aoIndex;
					aoIndex++;
					break;
				}
			
				// 4-20mA signal outputs
				if ((terminals[i] == 554) | 
					(terminals[i] == 555))
				{
					modules[i].type = "AO";
					modules[i].mtype = aom;
					modules[i].desc = "4-20mA analog output";
					modules[i].pn = terminals[i];
					modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
					modules[i].inChannelCount = 0;
					modules[i].outChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
					modules[i].bitOffsetOut = terminalDescription[i].OffsetOutput_bits;
					log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetOut);
			
					// map to type index
					modules[i].typeIndex = aoIndex;
					aoIndex++;
					break;
				}
			
				// 0/4-20mA signal outputs
				if (terminals[i] == 563)
				{
					modules[i].type = "AO";
					modules[i].mtype = aom;
					modules[i].desc = "0/4-20mA analog output";
					modules[i].pn = terminals[i];
					modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
					modules[i].inChannelCount = 0;
					modules[i].outChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
					modules[i].bitOffsetOut = terminalDescription[i].OffsetOutput_bits;
					log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetOut);
			
					// map to type index
					modules[i].typeIndex = aoIndex;
					aoIndex++;
					break;
				}
			
				// +/-10vdc signal outputs
				if ((terminals[i] == 556) | 
					(terminals[i] == 557))
				{
					modules[i].type = "AO";
					modules[i].mtype = aom;
					modules[i].desc = "+/-10vdc analog output";
					modules[i].pn = terminals[i];
					modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
					modules[i].inChannelCount = 0;
					modules[i].outChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
					modules[i].bitOffsetOut = terminalDescription[i].OffsetOutput_bits;
					log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetOut);
			
					// map to type index
					modules[i].typeIndex = aoIndex;
					aoIndex++;
					break;
				}
			
				// 0-10vdc signal outputs
				if ((terminals[i] == 550) | 
					(terminals[i] == 559) |
					(terminals[i] == 560))
				{
					modules[i].type = "AO";
					modules[i].mtype = aom;
					modules[i].desc = "0-10vdc analog output";
					modules[i].pn = terminals[i];
					modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
					modules[i].inChannelCount = 0;
					modules[i].outChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
					modules[i].bitOffsetOut = terminalDescription[i].OffsetOutput_bits;
					log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetOut);
			
					// map to type index
					modules[i].typeIndex = aoIndex;
					aoIndex++;
					break;
				}
			
				// 0-10vdc / +/-10vdc signal outputs
				if ((terminals[i] == 562) | 
					(terminals[i] == 597))
				{
					modules[i].type = "AO";
					modules[i].mtype = aom;
					modules[i].desc = "0-10vdc / +/-10vdc analog output";
					modules[i].pn = terminals[i];
					modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
					modules[i].inChannelCount = 0;
					modules[i].outChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
					modules[i].bitOffsetOut = terminalDescription[i].OffsetOutput_bits;
					log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetOut);
			
					// map to type index
					modules[i].typeIndex = aoIndex;
					aoIndex++;
					break;
				}
			
				// configurable analog output
				if (terminals[i] == 564)
				{
					modules[i].type = "AO";
					modules[i].mtype = aom;
					modules[i].desc = "configurable analog output";
					modules[i].pn = terminals[i];
					modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
					modules[i].inChannelCount = 0;
					modules[i].outChannelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
					modules[i].bitOffsetOut = terminalDescription[i].OffsetOutput_bits;
					log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetOut);
			
					// map to type index
					modules[i].typeIndex = aoIndex;
					aoIndex++;
					break;
				}
			
			
			//*******************************************************************************************/
			//				DIGITAL MODULES
			//*******************************************************************************************/	
			case 34000 ... 37000:			
				if ((CHECK_BIT(terminals[i], 0)) && (CHECK_BIT(terminals[i], 1))) {
					modules[i].type = "DX";
					modules[i].mtype = dxm;
					modules[i].desc = "digital input/output module";
					modules[i].pn = terminals[i];
					modules[i].channelCount = ((terminalDescription[i].SizeInput_bits) + (terminalDescription[i].SizeOutput_bits));
					modules[i].inChannelCount = (terminalDescription[i].SizeInput_bits);
					modules[i].outChannelCount = (terminalDescription[i].SizeOutput_bits);
					modules[i].bitOffsetOut = terminalDescription[i].OffsetOutput_bits;
					modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
					log_trace("Module %d is a %d channel %s at bit input offset %d, output offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn, modules[i].bitOffsetIn);
				
					// map to type index
					modules[i].typeIndex = dxIndex;
					dxIndex++;
					break;
				}
				if ((!CHECK_BIT(terminals[i], 0)) && (CHECK_BIT(terminals[i], 1))) {
					modules[i].type = "DO";
					modules[i].mtype = dom;
					modules[i].desc = "digital output module";
					modules[i].pn = terminals[i];
					modules[i].channelCount = (terminalDescription[i].SizeOutput_bits);
					modules[i].inChannelCount = 0;
					modules[i].outChannelCount = (terminalDescription[i].SizeOutput_bits);
					modules[i].bitOffsetOut = terminalDescription[i].OffsetOutput_bits;
					log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetOut);
				
					// map to type index
					modules[i].typeIndex = doIndex;
					doIndex++;
					break;
				}
				if ((CHECK_BIT(terminals[i], 0)) && (!CHECK_BIT(terminals[i], 1))) {
					modules[i].type = "DI";
					modules[i].mtype = dim;
					modules[i].desc = "digital input module";
					modules[i].pn = terminals[i];
					modules[i].channelCount = (terminalDescription[i].SizeInput_bits);
					modules[i].inChannelCount = (terminalDescription[i].SizeInput_bits);
					modules[i].outChannelCount = 0;
					modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
					log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
				
					// map to type index
					modules[i].typeIndex = diIndex;
					diIndex++;
					break;
				}
			
			// default to "unsupported module typeif it falls outside of this segment
			default :
				modules[i].type = "unsupported";
				modules[i].pn = terminals[i];
				log_error("Module %d is unsupported", i);
				break;
			}

		switch (modules[i].mtype)
		{
		case dim:
			for (int y = 0; y < modules[i].channelCount; y++)
			{
				diMod[modules[i].typeIndex].inData[y].label = return_default_labe(i,"_input", y);
			}
			break;
		case dom:
			for (int y = 0; y < modules[i].channelCount; y++)
			{
				doMod[modules[i].typeIndex].outData[y].label = return_default_labe(i, "_output", y);
			}
			break;
		case dxm:
			for (int y = 0; y < modules[i].inChannelCount; y++)
			{
				dxMod[modules[i].typeIndex].inData[y].label = return_default_labe(i, "_input", y);
			}
			for (int y = 0; y < modules[i].outChannelCount; y++)
			{
				dxMod[modules[i].typeIndex].outData[y].label = return_default_labe(i, "_output", y);
			}
			break;
		case aim:
			for (int y = 0; y < modules[i].channelCount; y++)
			{
				diMod[modules[i].typeIndex].inData[y].label = return_default_labe(i, "_input", y);
			}
			break;
		case aom:
			for (int y = 0; y < modules[i].channelCount; y++)
			{
				aoMod[modules[i].typeIndex].outData[y].label = return_default_labe(i, "_output", y);
			}
			break;
		}
	}
	return 1;
}
