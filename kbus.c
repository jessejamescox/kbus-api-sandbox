//  This file is part of kbus_daemon.
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
#include <dal/adi_application_interface.h>
#include <ldkc_kbus_information.h>
#include <ldkc_kbus_register_communication.h>
#include <math.h>

#include "kbus.h"
#include "kbus-api.h"
#include "node.h"
#include "logger.h"
#include "json.h"
#include "get_config.h"

typedef union {
	float f;
	unsigned int h;
} hexfloat;

float bytes_to_float(unsigned int in)
{
	hexfloat hf;
	hf.h = in;
	float f = hf.f;
	printf("%f\n", f);
	return f;
}

#define CHK_BIT(var,pos) ((var) &   (1<<(pos)))
#define SET_BIT(var,pos) ((var) |=  (1<<(pos)))
#define CLR_BIT(var,pos) ((var) &= ~(1<<(pos)))
#define FLP_BIT(var,pos) ((var) ^=  (1<<(pos)))

struct node controllerLast;

tApplicationDeviceInterface *adi;
struct kbus kbus;

// holding arrays for IO modules
struct pmMod pmMod[MAXPMMCOUNT];
struct diMod diMod[MAXDIGCOUNT];
struct doMod doMod[MAXDIGCOUNT];
struct dxMod dxMod[MAXDIGCOUNT];
struct aiMod aiMod[MAXALGCOUNT];
struct aoMod aoMod[MAXALGCOUNT];

bool initState = true;


int kbus_init(struct kbus *kbus) {
	//, tApplicationDeviceInterface *adi) {
	
	int i;
	
	adi = adi_GetApplicationInterface();
	
	// initilalize the kbus
	adi->Init();
	
	// get the device list
	log_info("Scanning the ADI devices looking for Kbus");
	adi->ScanDevices();
	adi->GetDeviceList(sizeof(kbus->deviceList), kbus->deviceList, &kbus->nrDevicesFound);	
	log_info("ADI found %d devices", kbus->nrDevicesFound);
	
	// scan Kbus for num of devices
	log_info("Scanning devices for Kbus");
	kbus->nrKbusFound = -1;
	for (i = 0; i < kbus->nrDevicesFound; i++) {
		if (strcmp(kbus->deviceList[i].DeviceName, "libpackbus") == 0) {
			kbus->nrKbusFound = i;
		}
	}
	
	// check for errors and exit if needed
	if (kbus->nrKbusFound == -1) {			
		log_error("No Kbus device found -- exited");
		adi->Exit();
		return -1;	// exit the program
	}
	else {
		log_info("Kbus device found as ADi device %d", kbus->nrKbusFound);
	}
	
	// open the kbus device
	log_info("Opening the Kbus");
	kbus->kbusDeviceId = kbus->deviceList[kbus->nrKbusFound].DeviceId;
	if (adi->OpenDevice(kbus->kbusDeviceId) != DAL_SUCCESS) {
		log_error("Failed to open Kbus device -- exited");
		adi->Exit();
		return -2;
	}
	
	// set the application to state "Running"
	log_info("Setting application state to 'Running'");
	kbus->event.State = ApplicationState_Running;
	if (adi->ApplicationStateChanged(kbus->event) != DAL_SUCCESS) {
		log_error("Failed to set Kbus application state -- exited");
		adi->CloseDevice(kbus->kbusDeviceId);
		adi->Exit();
		return -3;
	}

	// get the kbus info via the dbus
	log_info("Reading the Kbus info");
	if (KbusInfo_Failed == ldkc_KbusInfo_Create()) {
		log_error("Failed to get Kbus config via dbus -- exited");
		adi->CloseDevice(kbus->kbusDeviceId);
		adi->Exit();
		return -11;
	}
		
	// get the Kbus status
	log_info("Reading Kbus status");
	tldkc_KbusInfo_Status status;
	if (KbusInfo_Failed == ldkc_KbusInfo_GetStatus(&status)) {
		log_error("Failed to get Kbus status -- exited");
		adi->CloseDevice(kbus->kbusDeviceId);
		adi->Exit();
		ldkc_KbusInfo_Destroy();
	}
	
	// get the module descriptions by position
	log_info("Successfully openend Kbus, reading module informatioon by position");
	if (KbusInfo_Failed == ldkc_KbusInfo_GetTerminalInfo(OS_ARRAY_SIZE(kbus->terminalDescription), kbus->terminalDescription, &kbus->terminalCount)) {
		log_error("Failed to get Kbus module information -- exited");
		adi->CloseDevice(kbus->kbusDeviceId);
		adi->Exit();
		ldkc_KbusInfo_Destroy();
		return -13;
	}
	else {
		log_info("Found %d modules", kbus->terminalCount);
	}
	
	// get the kbus size and offsets   
	if (KbusInfo_Failed == ldkc_KbusInfo_GetTerminalList(OS_ARRAY_SIZE(kbus->terminals), kbus->terminals, NULL)) {
		log_error("Failed to get terminal list -- exited");
		adi->CloseDevice(kbus->kbusDeviceId); // close kbus device    
		adi->Exit(); // disconnect ADI-Interface 
		ldkc_KbusInfo_Destroy(); 
		return -14;
	}
}

int kbus_write_digital(int modulePosition, int channelPosition, bool channelValue) {
	adi->WriteStart(kbus.kbusDeviceId, kbus.taskId);
	adi->WriteBool(kbus.kbusDeviceId, kbus.taskId, (controller.modules[modulePosition].bitOffsetOut + channelPosition), channelValue);
	adi->WriteEnd(kbus.kbusDeviceId, kbus.taskId);  
	doMod[controller.modules[modulePosition].typeIndex].outData[channelPosition].value = channelValue;
}

int kbus_write_analog(int modulePosition, int channelPosition, uint16_t channelValue) {
	
	// calcualte the byte offset 
	//int byteOffset;// = controller;
	int byteOffset = ((controller.modules[modulePosition].bitOffsetOut / 8) + (channelPosition * 2)) ;
	
	adi->WriteStart(kbus.kbusDeviceId, kbus.taskId);
	adi->WriteBytes(kbus.kbusDeviceId, kbus.taskId, byteOffset, 2, (uint16_t *)  &channelValue);
	adi->WriteEnd(kbus.kbusDeviceId, kbus.taskId);  
	aoMod[controller.modules[modulePosition].typeIndex].outData[channelPosition].value = channelValue;
}

int kbus_write_pmm(int modulePosition, uint8_t outdata[8])
{	
	//uint8_t *outHold = &outdata; 
	int byteOffset = (controller.modules[modulePosition].bitOffsetOut / 8);
	// now send the request
	adi->WriteStart(kbus.kbusDeviceId, kbus.taskId);
	adi->WriteBytes(kbus.kbusDeviceId, kbus.taskId, byteOffset, 8, (uint8_t *)  &outdata[0]);
	adi->WriteEnd(kbus.kbusDeviceId, kbus.taskId);
	return 0;
}


bool kbus_read_digital(int *i_modules, int *i_channels, struct kbus *kbus)
{
	int iMod =  i_modules;
	int iChan = i_channels;
	
	bool xOut;
	// read inputs by channel		            
	adi->ReadStart(kbus->kbusDeviceId, kbus->taskId); // lock PD-In data 
	adi->ReadBool(kbus->kbusDeviceId, kbus->taskId, (kbus->terminalDescription[iMod].OffsetInput_bits + iChan), &xOut);
	adi->ReadEnd(kbus->kbusDeviceId, kbus->taskId); // unlock PD-In data 
	return xOut;
}





int kbus_read_analog(int *i_modules, int *i_channels, struct kbus *kbus)
{
	uint8_t iMod =  i_modules;
	uint8_t iChan = i_channels;
	
	int xOut;
	// read inputs by channel		            
	int byteOffset = ((controller.modules[iMod].bitOffsetIn / 8) + (iChan * 2));
	adi->ReadStart(kbus->kbusDeviceId, kbus->taskId); // lock PD-In data 
	adi->ReadBytes(kbus->kbusDeviceId, kbus->taskId, byteOffset, 2, (uint16_t *) &xOut);
	adi->ReadEnd(kbus->kbusDeviceId, kbus->taskId); // unlock PD-In data
	return xOut;
}

uint8_t *kbus_read_bytes(int *i_modules, struct kbus *kbus, uint8_t indata[24])
{
	uint8_t iMod =  i_modules;
	
	// read inputs by channel		            
	int byteOffset = ((controller.modules[iMod].bitOffsetIn / 8));
	adi->ReadStart(kbus->kbusDeviceId, kbus->taskId); // lock PD-In data 
	adi->ReadBytes(kbus->kbusDeviceId, kbus->taskId, byteOffset, 24, (uint8_t *) &indata[0]);
	adi->ReadEnd(kbus->kbusDeviceId, kbus->taskId); // unlock PD-In data
}


void kbus_read_pmm(int *i_modules, struct kbus *kbus, struct pmMod *pmm)
{
	uint8_t iMod = i_modules;
	uint8_t iTi = controller.modules[iMod].typeIndex;
	
	uint8_t statusBytes[8] = { 0 }; // = { 0 };// = (uint8_t *) malloc(24);
	uint32_t dataRegs[4] = { 0 };
	
	uint8_t controlBytes[8] = { 0 }; // = (uint8_t *) malloc(24);
	
	// read 24 byte message from pmm pi            
	int byteOffset = (controller.modules[iMod].bitOffsetIn / 8);
	adi->ReadStart(kbus->kbusDeviceId, kbus->taskId); // lock PD-In data 
	adi->ReadBytes(kbus->kbusDeviceId, kbus->taskId, byteOffset, 8, (uint8_t *) &statusBytes[0]);
	adi->ReadBytes(kbus->kbusDeviceId, kbus->taskId, (byteOffset + 8), 16, (uint32_t *) &dataRegs[0]);
	adi->ReadEnd(kbus->kbusDeviceId, kbus->taskId); // unlock PD-In data
	
	controlBytes[0] = 0;
	controlBytes[2] = NULL;
	controlBytes[3] = 10;
	
	// getting the L1 data
	if ((statusBytes[4] == 4) &
		(statusBytes[5] == 1) & 
		(statusBytes[6] == 7) &
		(statusBytes[7] == 16))
	{
		
		pmMod[iTi].L1.measuredVolts		= dataRegs[0];
		pmMod[iTi].L1.measuredAmps		= dataRegs[1];
		pmMod[iTi].L1.measuredPower		= dataRegs[2];
		pmMod[iTi].L1.measuredFrequency = dataRegs[3];
		
		pmMod[iTi].L1.errorUnderVolts		= CHK_BIT(statusBytes[2], 1);
		pmMod[iTi].L1.errorCurrentRange		= CHK_BIT(statusBytes[2], 2);
		pmMod[iTi].L1.errorVoltsRange		= CHK_BIT(statusBytes[2], 3);
		pmMod[iTi].L1.errorZeroCross		= CHK_BIT(statusBytes[2], 4);
		pmMod[iTi].L1.errorOverCurrent		= CHK_BIT(statusBytes[2], 5);
		pmMod[iTi].L1.errorFieldCCW			= CHK_BIT(statusBytes[2], 6);
		pmMod[iTi].L1.errorUnderVolts		= CHK_BIT(statusBytes[2], 7);
			
		controlBytes[1] = 1; // query L2
		controlBytes[4] = 5;
		controlBytes[5] = 2;
		controlBytes[6] = 8;
		controlBytes[7] = 17;
	}
	
	// getting the L2 data
	else if((statusBytes[4] == 5) &
		(statusBytes[5] == 2) &
		(statusBytes[6] == 8) &
		(statusBytes[7] == 17))
	{
		pmMod[iTi].L2.measuredVolts		= dataRegs[0];
		pmMod[iTi].L2.measuredAmps		= dataRegs[1];
		pmMod[iTi].L2.measuredPower		= dataRegs[2];
		pmMod[iTi].L2.measuredFrequency = dataRegs[3];
		
		pmMod[iTi].L2.errorUnderVolts		= CHK_BIT(statusBytes[2], 1);
		pmMod[iTi].L2.errorCurrentRange		= CHK_BIT(statusBytes[2], 2);
		pmMod[iTi].L2.errorVoltsRange		= CHK_BIT(statusBytes[2], 3);
		pmMod[iTi].L2.errorZeroCross		= CHK_BIT(statusBytes[2], 4);
		pmMod[iTi].L2.errorOverCurrent		= CHK_BIT(statusBytes[2], 5);
		pmMod[iTi].L2.errorFieldCCW			= CHK_BIT(statusBytes[2], 6);
		pmMod[iTi].L2.errorUnderVolts		= CHK_BIT(statusBytes[2], 7);
			
		controlBytes[1] = 2; // query L3
		controlBytes[4] = 6;
		controlBytes[5] = 3;
		controlBytes[6] = 9;
		controlBytes[7] = 18;
	}
	
	// getting the L3 data
	else if((statusBytes[4] == 6) &
		(statusBytes[5] == 3) &
		(statusBytes[6] == 9) &
		(statusBytes[7] == 18))
	{
		pmMod[iTi].L3.measuredVolts		= dataRegs[0];
		pmMod[iTi].L3.measuredAmps		= dataRegs[1];
		pmMod[iTi].L3.measuredPower		= dataRegs[2];
		pmMod[iTi].L3.measuredFrequency = dataRegs[3];
		
		pmMod[iTi].L3.errorUnderVolts		= CHK_BIT(statusBytes[2], 1);
		pmMod[iTi].L3.errorCurrentRange		= CHK_BIT(statusBytes[2], 2);
		pmMod[iTi].L3.errorVoltsRange		= CHK_BIT(statusBytes[2], 3);
		pmMod[iTi].L3.errorZeroCross		= CHK_BIT(statusBytes[2], 4);
		pmMod[iTi].L3.errorOverCurrent		= CHK_BIT(statusBytes[2], 5);
		pmMod[iTi].L3.errorFieldCCW			= CHK_BIT(statusBytes[2], 6);
		pmMod[iTi].L3.errorUnderVolts		= CHK_BIT(statusBytes[2], 7);
			
		controlBytes[1] = 0; // query L3
		controlBytes[3] = 10; // get AC vals
		controlBytes[4] = 4;
		controlBytes[5] = 1;
		controlBytes[6] = 7;
		controlBytes[7] = 16;
	}
	else
	{
		controlBytes[1] = 0; // query L3
		controlBytes[3] = 10; // get AC vals
		controlBytes[4] = 4;
		controlBytes[5] = 1;
		controlBytes[6] = 7;
		controlBytes[7] = 16;
	}
	
	// errors
	pmMod[iTi].L1.errorGeneral = CHK_BIT(statusBytes[0], 0);
	pmMod[iTi].L2.errorGeneral = CHK_BIT(statusBytes[0], 1);
	pmMod[iTi].L3.errorGeneral = CHK_BIT(statusBytes[0], 2);	
	
	pmMod[iTi].grpError = CHK_BIT(statusBytes[0], 3);
	pmMod[iTi].genError = CHK_BIT(statusBytes[0], 6);
	
	
	// now send the request
	kbus_write_pmm(iMod, controlBytes);
	
}


int kbus_read(struct mosquitto *mosq, struct prog_config *this_config, struct kbus *kbus){//, struct node controller) {

	// create the error watch object
	uint32_t retval = 0;
      
	// use function "libpackbus_Push" to trigger one KBUS cycle.
	if (adi->CallDeviceSpecificFunction("libpackbus_Push", &retval) != DAL_SUCCESS) {
		// CallDeviceSpecificFunction failed
		printf("CallDeviceSpecificFunction failed\n");  
		adi->CloseDevice(kbus->kbusDeviceId); // close kbus device    
		adi->Exit(); // disconnect ADI-Interface      
		return -4;       // exit programm
	}
		
	if (retval != DAL_SUCCESS) {
		// Function 'libpackbus_Push' failed
		printf("Function 'libpackbus_Push' failed\n");  
		adi->CloseDevice(kbus->kbusDeviceId); // close kbus device    
		adi->Exit(); // disconnect ADI-Interface      
		return -5;         // exit programm
	}

	// Trigger Watchdog although this is currently unused
	adi->WatchdogTrigger();
	
	int i_modules, i_channels = 0;
	
	// read each channel of each module connected
	for (i_modules = 0; i_modules < kbus->terminalCount; i_modules++) {
		for (i_channels = 0; i_channels < controller.modules[i_modules].channelCount; i_channels++) {
			
			switch (controller.modules[i_modules].mtype)
			{
			case dim:
				diMod[controller.modules[i_modules].typeIndex].inData[i_channels].value = kbus_read_digital(i_modules, i_channels, kbus);
				break;
			case dxm:
				dxMod[controller.modules[i_modules].typeIndex].inData[i_channels].value = kbus_read_digital(i_modules, i_channels, kbus);
				break;
			case aim:
				aiMod[controller.modules[i_modules].typeIndex].inData[i_channels].value = kbus_read_analog(i_modules, i_channels, kbus);
				break;
			case spm:
				if ((controller.modules[i_modules].pn == 494) || (controller.modules[i_modules].pn == 495))
				{
					kbus_read_pmm(i_modules, kbus, &pmMod[controller.modules[i_modules].typeIndex]);
				}
				break;
			}
		}
	}
	return 0;
}

int kbus_write(struct mosquitto *mosq, struct node controller, int modulePosition, int channelPosition, int channelValue) 
{
	switch (controller.modules[modulePosition].mtype)
	{
	bool writeVal = false;
	case dxm:
	case dom:
		if (channelValue != 0) {
			writeVal = true;
		}
		kbus_write_digital(modulePosition, channelPosition, writeVal);
		break;
	case aom:
		kbus_write_analog(modulePosition, channelPosition, channelValue);
		break;
	default:
		break;
	}
}