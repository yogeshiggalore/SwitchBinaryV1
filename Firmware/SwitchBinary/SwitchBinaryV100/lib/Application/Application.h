/*******************************************************************************
* File Name: Application.h
*
* Version: 1.0
*
* Description:
* This is header file for application. All the functions related to
* application are implemented in this file
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
#ifndef Application_h
#define Application_h

#include <Arduino.h>

#define GREEN_LED       1
#define YELLOW_LED      2
#define RELAY           3

#define PIN_GREEN_LED      12 
#define PIN_YELLOW_LED     13
#define PIN_SB_RELAY   14   

class Application{
public:
    Application();
    uint8_t ui8GreenLedStatus=0;
    uint8_t ui8YellowLedStatus=0;
    uint8_t ui8MqttStatus=0;
    uint8_t ui8AlexaStatus=0;
    uint8_t ui8RelayStatus=0;
    uint8_t ui8RTCTimeConfigStatus=0;
    
    uint8_t aui8TimeConfig[128];
    uint8_t aui8OTTimeConfig[8];

    uint8_t ui8AlexaUpdated=0;
    void Start(void);
    void Application_Pin_Write(uint8_t ui8Pin, uint8_t ui8State);
    uint8_t Application_Pin_Read(uint8_t ui8Pin);
    void HandleAlexa(uint8_t * payload);
    uint8_t Check_For_Device_ID(String sDeviceID);
    void Check_For_Action(String sAction, String sValue, int i16jsonValue, int* ai16ResponseData);
    void Check_RTC_TimeConfig(void);
    String Get_Timeconfig(uint8_t ui8Index, uint8_t ui8Type);
    String Get_Timeconfig_Complete(uint8_t ui8Index);
    
    String Get_OTTimeconfig( uint8_t ui8Type);
    String Get_OTTimeconfig_Complete(void);

    uint8_t Set_Timeconfig_Complete(uint8_t ui8Index,String sData);
    uint8_t Set_OTTimeconfig_Complete(String sData);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern Application EApp;
#endif
#endif