/*******************************************************************************
* File Name: ModbusSlave.h
*
* Version: 1.0
*
* Description:
* This is header file for memory. All the functions related to
* Memory are implemented in this file
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
#ifndef ESPMemory_h
#define ESPMemory_h

#include <Arduino.h>

#define MEMORY_SIZE       150

#define MEMORY_RELAY_STATUS_BYTE_INDEX    128
#define MEMORY_RTC_TIMECONFIG_BYTE_INDEX  129
  
#define MEMORY_TIMECONFIG_ACTIVE_INDEX    0
#define MEMORY_TIMECONFIG_SHOUR_INDEX     1
#define MEMORY_TIMECONFIG_SMINT_INDEX     2  
#define MEMORY_TIMECONFIG_SSECD_INDEX     3
#define MEMORY_TIMECONFIG_EHOUR_INDEX     4
#define MEMORY_TIMECONFIG_EMINT_INDEX     5  
#define MEMORY_TIMECONFIG_ESECD_INDEX     6
#define MEMORY_TIMECONFIG_WEEK_INDEX      7

#define MEMORY_OTTIMECONFIG_ACTIVE_INDEX    130
#define MEMORY_OTTIMECONFIG_SHOUR_INDEX     131
#define MEMORY_OTTIMECONFIG_SMINT_INDEX     132
#define MEMORY_OTTIMECONFIG_SSECD_INDEX     133
#define MEMORY_OTTIMECONFIG_EHOUR_INDEX     134
#define MEMORY_OTTIMECONFIG_EMINT_INDEX     135  
#define MEMORY_OTTIMECONFIG_ESECD_INDEX     136
#define MEMORY_OTTIMECONFIG_WEEK_INDEX      137

#define MEMORY_INIT_BYTE       138

#define TIMECONFIG_BYTE_SIZE    128

class ESPMemory{
public:
    ESPMemory();
    
    void Init(void);
    void Update_All(void);

    void Set_Timeconfig(uint8_t ui8SHour,uint8_t ui8SMinute,uint8_t ui8SSecond, uint8_t ui8EHour,uint8_t ui8EMinute,uint8_t ui8ESecond,uint8_t ui8Index, uint8_t ui8Enable,uint8_t ui8Week);
    void Set_OTTimeconfig(uint8_t ui8SHour,uint8_t ui8SMinute,uint8_t ui8SSecond, uint8_t ui8EHour,uint8_t ui8EMinute,uint8_t ui8ESecond,uint8_t ui8Enable,uint8_t ui8Week);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern ESPMemory EMemory;
#endif
#endif