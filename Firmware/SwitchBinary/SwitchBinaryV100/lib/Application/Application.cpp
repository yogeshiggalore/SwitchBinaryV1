/*******************************************************************************
* File Name: Application.cpp
*
* Version: 1.0
*
* Description:
* This is source file for application. All the functions related to
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

#include <Arduino.h>
#include <ESPUtils.h>
#include <Application.h>
#include <EEPROM.h>
#include <FS.h>
#include <ESPMemory.h>
#include <ESPFile.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h> 
#include <ESPAlexaV2.h>
#include <ESPRTC.h>

// Constructors ////////////////////////////////////////////////////////////////
Application::Application(){}

void Application::Start(void){
    pinMode(PIN_GREEN_LED,OUTPUT);
    pinMode(PIN_YELLOW_LED,OUTPUT);
    pinMode(PIN_SB_RELAY,OUTPUT);

    digitalWrite(PIN_GREEN_LED,ui8RelayStatus);
    digitalWrite(PIN_YELLOW_LED,ui8YellowLedStatus);
    digitalWrite(PIN_SB_RELAY,ui8RelayStatus);
}

void Application::Application_Pin_Write(uint8_t ui8Pin, uint8_t ui8State){
    if(ui8Pin == GREEN_LED){
        if(ui8State){
            digitalWrite(PIN_GREEN_LED,HIGH);
            ui8GreenLedStatus = HIGH; 
        }else{
            digitalWrite(PIN_GREEN_LED,LOW);
            ui8GreenLedStatus = LOW; 
        }
    }else if(ui8Pin == YELLOW_LED){
        if(ui8State){
            digitalWrite(PIN_YELLOW_LED,HIGH);
            ui8YellowLedStatus = HIGH; 
        }else{
            digitalWrite(PIN_YELLOW_LED,LOW);
            ui8YellowLedStatus = LOW; 
        }
    }else if(ui8Pin == RELAY){
        if(ui8State){
            digitalWrite(PIN_SB_RELAY,HIGH);
            digitalWrite(PIN_GREEN_LED,HIGH);
            ui8RelayStatus = HIGH; 
            ui8GreenLedStatus = HIGH;
        }else{
            digitalWrite(PIN_SB_RELAY,LOW);
            digitalWrite(PIN_GREEN_LED,LOW);
            ui8RelayStatus = LOW; 
            ui8GreenLedStatus = LOW;
        }
        EEPROM.write(MEMORY_RELAY_STATUS_BYTE_INDEX,ui8RelayStatus);
        EEPROM.commit();
    }else{

    }    
}

uint8_t Application::Application_Pin_Read(uint8_t ui8Pin){
    uint8_t ui8Response=0;

    if(ui8Pin == GREEN_LED){
        ui8Response = ui8GreenLedStatus;
    }else if(ui8Pin == YELLOW_LED){
        ui8Response = ui8YellowLedStatus;
    }else if(ui8Pin == RELAY){
        ui8Response = ui8RelayStatus;
    }else{

    }

    return ui8Response;
}

String Application::Get_Timeconfig(uint8_t ui8Index, uint8_t ui8Type){
    String sData;
    uint8_t ui8Hour;
    uint8_t ui8Mint;
    uint8_t ui8Sec;

    if(ui8Type == 0){
        ui8Hour = aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_SHOUR_INDEX];
        ui8Mint = aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_SMINT_INDEX];
        ui8Sec  = aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_SSECD_INDEX];
    }else{
        ui8Hour = aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_EHOUR_INDEX];
        ui8Mint = aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_EMINT_INDEX];
        ui8Sec  = aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_ESECD_INDEX];
    }
    
    if(ui8Hour < 10){
        sData = "0" + String(ui8Hour) + ":";
    }else{
        sData = String(ui8Hour) + ":";
    }

    if(ui8Mint < 10){
        sData = sData + "0" + String(ui8Mint) + ":";
    }else{
        sData = sData + String(ui8Mint) + ":";
    }

    if(ui8Sec < 10){
        sData = sData + "0" + String(ui8Sec);
    }else{
        sData = sData + String(ui8Sec);
    }

    //Serial1.println(sData);
    return sData;
}

String Application::Get_Timeconfig_Complete(uint8_t ui8Index){
    String sData;
    uint8_t ui8SHour;
    uint8_t ui8SMint;
    uint8_t ui8SSec;
    uint8_t ui8EHour;
    uint8_t ui8EMint;
    uint8_t ui8ESec;
    uint8_t ui8Active;
    uint8_t ui8Week;

    ui8Active = aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX];
    ui8SHour = aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_SHOUR_INDEX];
    ui8SMint = aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_SMINT_INDEX];
    ui8SSec  = aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_SSECD_INDEX];
    ui8EHour = aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_EHOUR_INDEX];
    ui8EMint = aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_EMINT_INDEX];
    ui8ESec  = aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_ESECD_INDEX];
    ui8Week  = aui8TimeConfig[(ui8Index * 8) + MEMORY_TIMECONFIG_WEEK_INDEX];

    sData = "0" + String(ui8Active);
    if(ui8SHour < 10){
        sData = sData + "0" + String(ui8SHour);
    }else{
        sData = sData + String(ui8SHour);
    }

    if(ui8SMint < 10){
        sData = sData + "0" + String(ui8SMint);
    }else{
        sData = sData + String(ui8SMint);
    }

    if(ui8SSec < 10){
        sData = sData + "0" + String(ui8SSec);
    }else{
        sData = sData + String(ui8SSec);
    }
    if(ui8EHour < 10){
        sData = sData + "0" + String(ui8EHour);
    }else{
        sData = sData + String(ui8EHour);
    }

    if(ui8EMint < 10){
        sData = sData + "0" + String(ui8EMint);
    }else{
        sData = sData + String(ui8EMint);
    }

    if(ui8ESec < 10){
        sData = sData + "0" + String(ui8ESec);
    }else{
        sData = sData + String(ui8ESec);
    }

    if(ui8Week < 10){
        sData = sData + "0" + String(ui8Week);
    }else{
        sData = sData + String(ui8Week);
    }

    //Serial1.println(sData);
    return sData;
}

String Application::Get_OTTimeconfig(uint8_t ui8Type){
    String sData;
    uint8_t ui8Hour;
    uint8_t ui8Mint;
    uint8_t ui8Sec;

    if(ui8Type == 0){
        ui8Hour = aui8OTTimeConfig[+ MEMORY_TIMECONFIG_SHOUR_INDEX];
        ui8Mint = aui8OTTimeConfig[ MEMORY_TIMECONFIG_SMINT_INDEX];
        ui8Sec  = aui8OTTimeConfig[ MEMORY_TIMECONFIG_SSECD_INDEX];
    }else{
        ui8Hour = aui8OTTimeConfig[ MEMORY_TIMECONFIG_EHOUR_INDEX];
        ui8Mint = aui8OTTimeConfig[ MEMORY_TIMECONFIG_EMINT_INDEX];
        ui8Sec  = aui8OTTimeConfig[ MEMORY_TIMECONFIG_ESECD_INDEX];
    }
    
    if(ui8Hour < 10){
        sData = "0" + String(ui8Hour) + ":";
    }else{
        sData = String(ui8Hour) + ":";
    }

    if(ui8Mint < 10){
        sData = sData + "0" + String(ui8Mint) + ":";
    }else{
        sData = sData + String(ui8Mint) + ":";
    }

    if(ui8Sec < 10){
        sData = sData + "0" + String(ui8Sec);
    }else{
        sData = sData + String(ui8Sec);
    }

    //Serial1.println(sData);
    return sData;
}

String Application::Get_OTTimeconfig_Complete(void){
    String sData;
    uint8_t ui8SHour;
    uint8_t ui8SMint;
    uint8_t ui8SSec;
    uint8_t ui8EHour;
    uint8_t ui8EMint;
    uint8_t ui8ESec;
    uint8_t ui8Active;
    uint8_t ui8Week;

    ui8Active = aui8OTTimeConfig[MEMORY_TIMECONFIG_ACTIVE_INDEX];
    ui8SHour = aui8OTTimeConfig[MEMORY_TIMECONFIG_SHOUR_INDEX];
    ui8SMint = aui8OTTimeConfig[MEMORY_TIMECONFIG_SMINT_INDEX];
    ui8SSec  = aui8OTTimeConfig[MEMORY_TIMECONFIG_SSECD_INDEX];
    ui8EHour = aui8OTTimeConfig[MEMORY_TIMECONFIG_EHOUR_INDEX];
    ui8EMint = aui8OTTimeConfig[MEMORY_TIMECONFIG_EMINT_INDEX];
    ui8ESec  = aui8OTTimeConfig[MEMORY_TIMECONFIG_ESECD_INDEX];
    ui8Week  = aui8OTTimeConfig[MEMORY_TIMECONFIG_WEEK_INDEX];

    sData = "0" + String(ui8Active);
    if(ui8SHour < 10){
        sData = sData + "0" + String(ui8SHour);
    }else{
        sData = sData + String(ui8SHour);
    }

    if(ui8SMint < 10){
        sData = sData + "0" + String(ui8SMint);
    }else{
        sData = sData + String(ui8SMint);
    }

    if(ui8SSec < 10){
        sData = sData + "0" + String(ui8SSec);
    }else{
        sData = sData + String(ui8SSec);
    }
    if(ui8EHour < 10){
        sData = sData + "0" + String(ui8EHour);
    }else{
        sData = sData + String(ui8EHour);
    }

    if(ui8EMint < 10){
        sData = sData + "0" + String(ui8EMint);
    }else{
        sData = sData + String(ui8EMint);
    }

    if(ui8ESec < 10){
        sData = sData + "0" + String(ui8ESec);
    }else{
        sData = sData + String(ui8ESec);
    }

    if(ui8Week < 10){
        sData = sData + "0" + String(ui8Week);
    }else{
        sData = sData + String(ui8Week);
    }

    //Serial1.println(sData);
    return sData;
}

uint8_t Application::Set_Timeconfig_Complete(uint8_t ui8Index,String sData){
    uint8_t ui8Response=0;
    uint8_t ui8SHour;
    uint8_t ui8SMint;
    uint8_t ui8SSec;
    uint8_t ui8EHour;
    uint8_t ui8EMint;
    uint8_t ui8ESec;
    uint8_t ui8Active;
    uint8_t ui8Week;

    String  sString;

    if(sData.length() > 15){
        sData = sData.substring(0,16);
        Serial1.println(sData);
        Serial1.println(sData.length());
    }

    if(sData.length() == 16){
        sString = sData.substring(0,2);
        ui8Active = sString.toInt();

        sString = sData.substring(2,4);
        ui8SHour = sString.toInt();
        sString = sData.substring(4,6);
        ui8SMint = sString.toInt();
        sString = sData.substring(6,8);
        ui8SSec = sString.toInt();
        sString = sData.substring(8,10);
        ui8EHour = sString.toInt();
        sString = sData.substring(10,12);
        ui8EMint = sString.toInt();
        sString = sData.substring(12,14);
        ui8ESec = sString.toInt();
        sString = sData.substring(14,16);
        ui8Week = sString.toInt();

        Serial1.println(ui8SHour);
        Serial1.println(ui8SMint);
        Serial1.println(ui8SSec);
        Serial1.println(ui8EHour);
        Serial1.println(ui8EMint);
        Serial1.println(ui8ESec);
        Serial1.println(ui8Active);
        Serial1.println(ui8Week);

        if((ui8Index < 16) && (ui8Active < 2)){
            if((ui8SHour < 23) && (ui8SMint < 60) && (ui8SSec < 60)){
                if((ui8EHour < 23) && (ui8EMint < 60) && (ui8ESec < 60)){
                    EMemory.Set_Timeconfig(ui8SHour,ui8SMint,ui8SSec,ui8EHour,ui8EMint,ui8ESec,ui8Index,ui8Active,ui8Week);
                    ui8Response = 1;
                    EMemory.Update_All();
                    Serial1.println("Sucess in mqtt save");
                }else{
                    Serial1.println("Error in mqtt timeconfig");
                }    
            }else{
                Serial1.println("Error in mqtt timeconfig");
            }    
        }else{
            Serial1.println("Error in mqtt timeconfig");
        }
    }else{
        ui8Response = 0;
        Serial1.println("Error in mqtt save");
    }

    return ui8Response;
}

uint8_t Application::Set_OTTimeconfig_Complete(String sData){
    uint8_t ui8Response=0;
    uint8_t ui8SHour;
    uint8_t ui8SMint;
    uint8_t ui8SSec;
    uint8_t ui8EHour;
    uint8_t ui8EMint;
    uint8_t ui8ESec;
    uint8_t ui8Active;
    uint8_t ui8Week;

    String  sString;

    if(sData.length() > 15){
        sData = sData.substring(0,16);
        Serial1.println(sData);
        Serial1.println(sData.length());
    }

    if(sData.length() == 16){
        sString = sData.substring(0,2);
        ui8Active = sString.toInt();

        sString = sData.substring(2,4);
        ui8SHour = sString.toInt();
        sString = sData.substring(4,6);
        ui8SMint = sString.toInt();
        sString = sData.substring(6,8);
        ui8SSec = sString.toInt();
        sString = sData.substring(8,10);
        ui8EHour = sString.toInt();
        sString = sData.substring(10,12);
        ui8EMint = sString.toInt();
        sString = sData.substring(12,14);
        ui8ESec = sString.toInt();
        sString = sData.substring(14,16);
        ui8Week = sString.toInt();

        Serial1.println(ui8SHour);
        Serial1.println(ui8SMint);
        Serial1.println(ui8SSec);
        Serial1.println(ui8EHour);
        Serial1.println(ui8EMint);
        Serial1.println(ui8ESec);
        Serial1.println(ui8Active);
        Serial1.println(ui8Week);

        if(ui8Active < 2){
            if((ui8SHour < 23) && (ui8SMint < 60) && (ui8SSec < 60)){
                if((ui8EHour < 23) && (ui8EMint < 60) && (ui8ESec < 60)){
                    EMemory.Set_OTTimeconfig(ui8SHour,ui8SMint,ui8SSec,ui8EHour,ui8EMint,ui8ESec,ui8Active,ui8Week);
                    ui8Response = 1;
                    EMemory.Update_All();
                    Serial1.println("Sucess in mqtt save");
                }else{
                    Serial1.println("Error in mqtt timeconfig");
                }    
            }else{
                Serial1.println("Error in mqtt timeconfig");
            }    
        }else{
            Serial1.println("Error in mqtt timeconfig");
        }
    }else{
        ui8Response = 0;
        Serial1.println("Error in mqtt save");
    }

    return ui8Response;
}


void Application::HandleAlexa(uint8_t * payload){
    DynamicJsonDocument json(1024);
    deserializeJson(json, payload,DeserializationOption::NestingLimit(20));
    uint8_t ui8DeviceIndex=0;

    String sDeviceId = json ["deviceId"];
    String sAction = json ["action"];
    String sValue = json["value"];
    int    i16jsonValue=0;
    int ai16ResponseData[3];

    ai16ResponseData[0] = 0x7FFF;
    ai16ResponseData[1] = 0x7FFF;
    ai16ResponseData[2] = 1;

    if(sAction == "SetPercentage"){
        i16jsonValue = json["value"]["percentage"];
    }else if(sAction == "AdjustPercentage"){
        i16jsonValue = json["value"]["percentageDelta"];
    }else if(sAction == "action.devices.commands.OnOff"){
        String sString1 = json["value"]["on"];
        EUtils.println(sString1);
        if(sString1 == "true"){
            i16jsonValue = 1;
            EUtils.println("OK");
        }else{
            i16jsonValue = 0;
            EUtils.println("Error");
        }
    }else if(sAction == "action.devices.commands.BrightnessAbsolute"){
        String sString1 = json["value"]["brightness"];
        EUtils.println(sString1);
        i16jsonValue = json["value"]["brightness"];
        EUtils.println("brigthness");
        EUtils.println(i16jsonValue);
    }else{
        i16jsonValue =  0;
    }

    EUtils.print("Sinric data: ");
    EUtils.print(sDeviceId);
    EUtils.print(" ");
    EUtils.print(sAction);
    EUtils.print(" ");
    EUtils.print(sValue);
    EUtils.print(" ");
    EUtils.println(i16jsonValue);

    //serializeJsonPretty(json,sString);
    ui8DeviceIndex = Check_For_Device_ID(sDeviceId);

    if(ui8DeviceIndex != 0xFF){
        Check_For_Action(sAction,sValue,i16jsonValue,ai16ResponseData);
    }else{
        EUtils.print("No config id:");
        EUtils.println(sDeviceId);
    }
}

uint8_t Application::Check_For_Device_ID(String sDeviceID){
    uint8_t ui8Index=0xFF;
 
    if(sDeviceID.equals(MAlexa.sSBRelayId)){
        //EUtils.print("Device is correct:");
        //EUtils.println(MAlexa.sAlexaACInput1ID);
        EUtils.println(MAlexa.sSBRelayId);
        ui8Index =  1;
    }else{
        ui8Index=0xFF;
    }

    return ui8Index;
}

void Application::Check_For_Action(String sAction, String sValue,int i16jsonValue, int* ai16ResponseData){

    if(sAction.equals("setPowerState")){
        
        if(sValue.equals(DEVICE_ON)){
            Application_Pin_Write(RELAY, HIGH);
            ui8AlexaUpdated = 1;
        }else{
            Application_Pin_Write(RELAY, LOW);
            ui8AlexaUpdated = 1;
        }
    }else if(sAction == "action.devices.commands.OnOff"){
        if(i16jsonValue){
            Application_Pin_Write(RELAY, HIGH);
            ui8AlexaUpdated = 1;
        }else{
            Application_Pin_Write(RELAY, LOW);
            ui8AlexaUpdated = 1;
        }
    }else{
        // do nothing
        ai16ResponseData[2] = 0;
    }
}

void Application::Check_RTC_TimeConfig(void){
    uint8_t ui8IndexCounter=0;
    uint8_t ui8Active=0;
    uint8_t ui8StartHour=0;
    uint8_t ui8StartMint=0;
    uint8_t ui8StartSec=0;
    uint8_t ui8EndHour=0;
    uint8_t ui8EndMint=0;
    uint8_t ui8EndSec=0;
    uint8_t ui8TaskCompleted=0;
    uint32_t ui32StartSecond=0;
    uint32_t ui32EndSecond=0;
    uint32_t ui32CurrentSecond=0;
    uint8_t  ui8RelayConfigChange=0;
    uint8_t  ui8DayOfWeek=0;

    if(ui8RTCTimeConfigStatus){
        for(ui8IndexCounter=0;ui8IndexCounter<16;ui8IndexCounter++){
            if(ui8TaskCompleted == 0){
                ui8Active = aui8TimeConfig[(ui8IndexCounter * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX];
                if(ui8Active){
                    ui8StartHour = aui8TimeConfig[(ui8IndexCounter * 8) + MEMORY_TIMECONFIG_SHOUR_INDEX];
                    ui8StartMint = aui8TimeConfig[(ui8IndexCounter * 8) + MEMORY_TIMECONFIG_SMINT_INDEX];
                    ui8StartSec  = aui8TimeConfig[(ui8IndexCounter * 8) + MEMORY_TIMECONFIG_SSECD_INDEX];

                    ui8EndHour = aui8TimeConfig[(ui8IndexCounter * 8) + MEMORY_TIMECONFIG_EHOUR_INDEX];
                    ui8EndMint = aui8TimeConfig[(ui8IndexCounter * 8) + MEMORY_TIMECONFIG_EMINT_INDEX];
                    ui8EndSec  = aui8TimeConfig[(ui8IndexCounter * 8) + MEMORY_TIMECONFIG_ESECD_INDEX];

                    ui8DayOfWeek = aui8TimeConfig[(ui8IndexCounter * 8) + MEMORY_TIMECONFIG_WEEK_INDEX];

                    ui32StartSecond = EUtils.Time_To_Seconds(ui8StartHour,ui8StartMint,ui8StartSec);
                    ui32EndSecond   = EUtils.Time_To_Seconds(ui8EndHour,ui8EndMint,ui8EndSec);
                    ui32CurrentSecond = EUtils.Time_To_Seconds(ERTC.ui8Hour,ERTC.ui8Minute,ERTC.ui8Second);
                    
                    if(ui32CurrentSecond >= ui32StartSecond){
                        if(ui32CurrentSecond <= ui32EndSecond){
                            if(ui8DayOfWeek & ERTC.Get_dayOfWeek_Index(ERTC.ui8DayOfWeek)){
                                ui8TaskCompleted = 1;
                                ui8RelayConfigChange = 1;
                            }
                        }
                    }
                }
            }
        }

        if(ui8RelayConfigChange == 0){
            ui8Active = aui8OTTimeConfig[MEMORY_TIMECONFIG_ACTIVE_INDEX];
            if(ui8Active){
                ui8StartHour = aui8OTTimeConfig[ MEMORY_TIMECONFIG_SHOUR_INDEX];
                ui8StartMint = aui8OTTimeConfig[ MEMORY_TIMECONFIG_SMINT_INDEX];
                ui8StartSec  = aui8OTTimeConfig[ MEMORY_TIMECONFIG_SSECD_INDEX];

                ui8EndHour = aui8OTTimeConfig[MEMORY_TIMECONFIG_EHOUR_INDEX];
                ui8EndMint = aui8OTTimeConfig[MEMORY_TIMECONFIG_EMINT_INDEX];
                ui8EndSec  = aui8OTTimeConfig[MEMORY_TIMECONFIG_ESECD_INDEX];

                ui8DayOfWeek = aui8OTTimeConfig[MEMORY_TIMECONFIG_WEEK_INDEX];

                ui32StartSecond = EUtils.Time_To_Seconds(ui8StartHour,ui8StartMint,ui8StartSec);
                ui32EndSecond   = EUtils.Time_To_Seconds(ui8EndHour,ui8EndMint,ui8EndSec);
                ui32CurrentSecond = EUtils.Time_To_Seconds(ERTC.ui8Hour,ERTC.ui8Minute,ERTC.ui8Second);
                    
                if(ui32CurrentSecond >= ui32StartSecond){
                    if(ui32CurrentSecond <= ui32EndSecond){
                        if(ui8DayOfWeek & ERTC.Get_dayOfWeek_Index(ERTC.ui8DayOfWeek)){
                            ui8TaskCompleted = 1;
                            ui8RelayConfigChange = 1;
                        }
                    }else{
                        EEPROM.write((MEMORY_OTTIMECONFIG_ACTIVE_INDEX),0);
                        EEPROM.commit();
                        EMemory.Update_All();
                    }
                }
            }
        }

        if(ui8RelayConfigChange){
            if(ui8RelayStatus == 0){
                Application_Pin_Write(RELAY, HIGH);
            }
        }else{
            if(ui8RelayStatus){
                Application_Pin_Write(RELAY, LOW);
            }
        }
    }
}

// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
Application EApp;
#endif