/*******************************************************************************
* File Name: ESPMemory.cpp
*
* Version: 1.0
*
* Description:
* This is source file for memory. All the functions related to
* memory are implemented in this file
*
* Owner:
* Yogesh M Iggalore
*
* Code Tested With:
* 1. platformio and esp8266
*
********************************************************************************
* Copyright (2020-21) , SwitchBinary
********************************************************************************/

#include <Arduino.h>
#include <ESPUtils.h>
#include <EEPROM.h>
#include <ESPMemory.h>
#include <Application.h>

// Constructors ////////////////////////////////////////////////////////////////
ESPMemory::ESPMemory(){}

/* ini memory and update all bytes*/
void ESPMemory::Init(void){
    uint8_t ui8LoopCounter=0;

    EEPROM.begin(MEMORY_SIZE);
    if(EEPROM.read(MEMORY_INIT_BYTE) != 1){
        EEPROM.write(MEMORY_RELAY_STATUS_BYTE_INDEX,0);
        EEPROM.write(MEMORY_RTC_TIMECONFIG_BYTE_INDEX,0);
        
        EEPROM.write(MEMORY_OTTIMECONFIG_ACTIVE_INDEX,0);
        EEPROM.write(MEMORY_OTTIMECONFIG_SHOUR_INDEX,0);
        EEPROM.write(MEMORY_OTTIMECONFIG_SMINT_INDEX,0);
        EEPROM.write(MEMORY_OTTIMECONFIG_SSECD_INDEX,0);
        EEPROM.write(MEMORY_OTTIMECONFIG_EHOUR_INDEX,0);
        EEPROM.write(MEMORY_OTTIMECONFIG_EMINT_INDEX,0);
        EEPROM.write(MEMORY_OTTIMECONFIG_ESECD_INDEX,0);
        EEPROM.write(MEMORY_OTTIMECONFIG_WEEK_INDEX,0);

        for(ui8LoopCounter=0;ui8LoopCounter<TIMECONFIG_BYTE_SIZE;ui8LoopCounter++){
            EEPROM.write(ui8LoopCounter,0);
        }
        EEPROM.write(MEMORY_INIT_BYTE,1);
        EEPROM.commit();
    }

	/* update all memory bytes */
	Update_All();
}

/* This function updates all memory bytes */
void ESPMemory::Update_All(void){
    uint8_t ui8LoopCounter=0;

    for(ui8LoopCounter=0;ui8LoopCounter<TIMECONFIG_BYTE_SIZE;ui8LoopCounter++){
        EApp.aui8TimeConfig[ui8LoopCounter] = EEPROM.read(ui8LoopCounter);
    }
    EApp.ui8RelayStatus = EEPROM.read(MEMORY_RELAY_STATUS_BYTE_INDEX);
    EApp.ui8RTCTimeConfigStatus = EEPROM.read(MEMORY_RTC_TIMECONFIG_BYTE_INDEX);

    EApp.aui8OTTimeConfig[0] = EEPROM.read(MEMORY_OTTIMECONFIG_ACTIVE_INDEX);
    EApp.aui8OTTimeConfig[1] = EEPROM.read(MEMORY_OTTIMECONFIG_SHOUR_INDEX);
    EApp.aui8OTTimeConfig[2] = EEPROM.read(MEMORY_OTTIMECONFIG_SMINT_INDEX);
    EApp.aui8OTTimeConfig[3] = EEPROM.read(MEMORY_OTTIMECONFIG_SSECD_INDEX);
    EApp.aui8OTTimeConfig[4] = EEPROM.read(MEMORY_OTTIMECONFIG_EHOUR_INDEX);
    EApp.aui8OTTimeConfig[5] = EEPROM.read(MEMORY_OTTIMECONFIG_EMINT_INDEX);
    EApp.aui8OTTimeConfig[6] = EEPROM.read(MEMORY_OTTIMECONFIG_ESECD_INDEX);
    EApp.aui8OTTimeConfig[7] = EEPROM.read(MEMORY_OTTIMECONFIG_WEEK_INDEX);
}

void ESPMemory::Set_Timeconfig(uint8_t ui8SHour,uint8_t ui8SMinute,uint8_t ui8SSecond, uint8_t ui8EHour,uint8_t ui8EMinute,uint8_t ui8ESecond,uint8_t ui8Index, uint8_t ui8Enable,uint8_t ui8Week){
    uint8_t ui8LoopCounter=0;

    EApp.aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX] = ui8Enable;
    EApp.aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_SHOUR_INDEX] = ui8SHour;
    EApp.aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_SMINT_INDEX] = ui8SMinute;
    EApp.aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_SSECD_INDEX] = ui8SSecond;
    EApp.aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_EHOUR_INDEX] = ui8EHour;
    EApp.aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_EMINT_INDEX] = ui8EMinute;
    EApp.aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_ESECD_INDEX] = ui8ESecond;
    EApp.aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_WEEK_INDEX] = ui8Week;
    
    for(ui8LoopCounter=0;ui8LoopCounter<TIMECONFIG_BYTE_SIZE;ui8LoopCounter++){
        EEPROM.write(ui8LoopCounter,EApp.aui8TimeConfig[ui8LoopCounter]);
    }

    EEPROM.commit();

    Update_All();
}


void ESPMemory::Set_OTTimeconfig(uint8_t ui8SHour,uint8_t ui8SMinute,uint8_t ui8SSecond, uint8_t ui8EHour,uint8_t ui8EMinute,uint8_t ui8ESecond,uint8_t ui8Enable,uint8_t ui8Week){
    uint8_t ui8LoopCounter=0;

    EApp.aui8OTTimeConfig[ MEMORY_TIMECONFIG_ACTIVE_INDEX] = ui8Enable;
    EApp.aui8OTTimeConfig[ MEMORY_TIMECONFIG_SHOUR_INDEX] = ui8SHour;
    EApp.aui8OTTimeConfig[ MEMORY_TIMECONFIG_SMINT_INDEX] = ui8SMinute;
    EApp.aui8OTTimeConfig[ MEMORY_TIMECONFIG_SSECD_INDEX] = ui8SSecond;
    EApp.aui8OTTimeConfig[ MEMORY_TIMECONFIG_EHOUR_INDEX] = ui8EHour;
    EApp.aui8OTTimeConfig[ MEMORY_TIMECONFIG_EMINT_INDEX] = ui8EMinute;
    EApp.aui8OTTimeConfig[ MEMORY_TIMECONFIG_ESECD_INDEX] = ui8ESecond;
    EApp.aui8OTTimeConfig[ MEMORY_TIMECONFIG_WEEK_INDEX] = ui8Week;
    
    for(ui8LoopCounter=0;ui8LoopCounter<8;ui8LoopCounter++){
        EEPROM.write((MEMORY_OTTIMECONFIG_ACTIVE_INDEX + ui8LoopCounter),EApp.aui8OTTimeConfig[ui8LoopCounter]);
    }

    EEPROM.commit();

    Update_All();
}

// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
ESPMemory EMemory;
#endif