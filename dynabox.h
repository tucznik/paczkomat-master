/*
 * dynabox.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef DYNABOX_H_
#define DYNABOX_H_

#include "machine.h"
#define MAX_DOORS 						7

#define NUMBER_OF_FAULTS 				17
#define NO_FAULT						0x00
#define F01_LED							0x01
#define F02_DOOR						0x02
#define F03_OPTICAL_SWITCHES			0x03
#define F04_DOOR_OPENED_TOO_FAR			0x04
#define F05_ELECTROMAGNET				0x05
#define F06_CLOSE_THE_DOOR				0x06
#define F07_DOOR_NOT_OPEN				0x07
#define F08_ILLEGAL_OPENING				0x08
#define F10_MECHANICAL_WARNING			0x0A
#define F11_MECHANICAL_FAULT			0x0B
#define F12_POSITIONING					0x0C
#define F13_MAIN_DOOR					0x0D
#define F14_HOMING_FAILED				0x0E
#define F15_ILLEGAL_ROTATION			0x0F
#define F16_ORDER_REFUSED				0x10
#define F17_24V_MISSSING				0x11

class DynaboxData : public MachineData
{
public:
};

class Dynabox : public Machine
{
public:
	Dynabox();
	void EV_LEDChecked(DynaboxData* pdata);
	void EV_ElectromagnetChecked(DynaboxData* pdata);
	void LoadSupportedFunctions();
	void SaveParameters();
	void SetCurrentCommand(uint8_t command);
	void CommandCheckLed();
	void CommandCheckElectromagnet();
	void CommandShowStatusOnLed();
	void CommandGetSetState();
	void Parse(uint8_t* frame);
	void ParseCheckLed(uint8_t* frame);
	void ParseCheckElectromagnet(uint8_t* frame);
	void ParseGetSetState(uint8_t* frame);
	void TimeoutLed();
	void TimeoutDoor();
	void PCCheckTransoptorsGetStatus(uint8_t res);
	void SlavesPoll();
	void ReplyTimeout();
	uint8_t led_same_for_all;
private:
	void ST_Init(DynaboxData* pdata);
	void ST_CheckingLED(DynaboxData* pdata);
	void ST_CheckingElectromagnet(DynaboxData* pdata);
	void ST_Homing(DynaboxData* pdata);
	enum States {ST_INIT = 0, ST_CHECKING_LED, ST_CHECKING_ELECTROMAGNET, ST_HOMING, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&Dynabox::ST_Init)
		STATE_MAP_ENTRY(&Dynabox::ST_CheckingLED)
		STATE_MAP_ENTRY(&Dynabox::ST_CheckingElectromagnet)
		STATE_MAP_ENTRY(&Dynabox::ST_Homing)
	END_STATE_MAP
	uint8_t faults_to_led_map[NUMBER_OF_FAULTS + 1];
	uint8_t last_fault[MAX_DOORS + 1];
	void (Dynabox::*pcommand)();
	void (Dynabox::*pparse)(uint8_t* frame);
	void (Dynabox::*ptimeout)();

};

extern Dynabox dynabox;
extern DynaboxData dynabox_data;

#endif /* DYNABOX_H_ */
