/*******************************************************************************
*File Name: ESPRTC.cpp
*
* Version: 1.0
*
* Description:
* In this source code for managing rtc
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , SwitchBinary
*******************************************************************************/
#include <Arduino.h>
#include <ESPRTC.h>
#include <Wire.h>
#include <ESPUtils.h>

// Constructors ////////////////////////////////////////////////////////////////
ESPRTC::ESPRTC(){}

void ESPRTC::Start(void){

    Wire.begin();

    /*if (!isrunning()) {
        Serial1.println("RTC is NOT running!");
        ui8RTCStatus = RTC_RUN_ERROR;
    }else{
        ui8RTCStatus = RTC_RUN_OK;
        Serial1.println("RTC is running!");
    }*/

    /*ui8Year = 20;
    ui8Month = 6;
    ui8DayOfMonth = 1;
    ui8Hour = 12;
    ui8Minute = 29;
    ui8Second = 30;
    Write();*/
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}


uint8_t ESPRTC::isrunning(void) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();

  Wire.requestFrom(DS3231_I2C_ADDRESS, 1);
  uint8_t ss = Wire.read();
  return !(ss >> 7);
}

String ESPRTC::Read(void){
    String sDateTime;

    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    /* set DS3231 register pointer to 00h */
    Wire.write(0); 
    Wire.endTransmission();
    Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
    
    /* request seven bytes of data from DS3231 starting from register 00h */
    ui8Second     = BcdToDec(Wire.read() & 0x7f);
    ui8Minute     = BcdToDec(Wire.read());
    ui8Hour       = BcdToDec(Wire.read() & 0x3f);
    ui8DayOfWeek  = BcdToDec(Wire.read());
    ui8DayOfMonth = BcdToDec(Wire.read());
    ui8Month      = BcdToDec(Wire.read());
    ui8Year       = BcdToDec(Wire.read());


    if(ui8StartFlag){
        ui8StartFlag = 0;
        ui8DayOfWeek = Get_DayOfWeek_Formula(ui8DayOfMonth,ui8Month,ui8Year);
    }    

    if(ui8Hour == 0){
        if((ui8Minute > 0) && (ui8Minute < 1)){
            ui8DayOfWeek = Get_DayOfWeek_Formula(ui8DayOfMonth,ui8Month,ui8Year);
        }
    }
    
    sDateTime = String(ui8Year);
    if(ui8Month < 10){
        sDateTime = sDateTime + "0" + String(ui8Month);
    }else{
        sDateTime = sDateTime + String(ui8Month);
    }

    if(ui8DayOfMonth < 10){
        sDateTime = sDateTime + "0" + String(ui8DayOfMonth);
    }else{
        sDateTime = sDateTime + String(ui8DayOfMonth);
    }

    if(ui8Hour < 10){
        sDateTime = sDateTime + "0" + String(ui8Hour);
    }else{
        sDateTime = sDateTime + String(ui8Hour);
    }

    if(ui8Minute < 10){
        sDateTime = sDateTime + "0" + String(ui8Minute);
    }else{
        sDateTime = sDateTime + String(ui8Minute);
    }

    if(ui8Second < 10){
        sDateTime = sDateTime + "0" + String(ui8Second);
    }else{
        sDateTime = sDateTime + String(ui8Second);
    }

    Serial1.print(sDateTime);
    Serial1.print(" ");
    Serial1.println(daysOfTheWeek[ui8DayOfWeek]);

    return sDateTime;
}

void ESPRTC::Write(void){
    /* sets time and date data to DS3231 */
    Wire.beginTransmission(DS3231_I2C_ADDRESS);

    /* set next input to start at the seconds register */
    Wire.write(0);
    
    Wire.write(DecToBcd(ui8Second)); 
    Wire.write(DecToBcd(ui8Minute));
    Wire.write(DecToBcd(ui8Hour));
    Wire.write(DecToBcd(ui8DayOfWeek));
    Wire.write(DecToBcd(ui8DayOfMonth));
    Wire.write(DecToBcd(ui8Month));
    Wire.write(DecToBcd(ui8Year));

    Wire.endTransmission();
}

byte ESPRTC::DecToBcd(byte val){
    return( (val/10*16) + (val%10) );
}

byte ESPRTC::BcdToDec(byte val){
    return( (val/16*10) + (val%16) );
}

void ESPRTC::handleTiming(void){
    Read();    
}

void ESPRTC::Update_Time_NTP(void){
    
    EUtils.GetTime();
    ui8Year = 0;

    ui8Second = EUtils.Get_Seconds();
    ui8Minute = EUtils.Get_Minutes();
    ui8Hour   = EUtils.Get_Hours();
    ui8DayOfMonth = EUtils.Get_Date();
    ui8Month  = EUtils.Get_Month();
    ui8Year   = EUtils.Get_Year(); 
    //ui8DayOfWeek = EUtils.Get_DayOfWeek();
    ui8DayOfWeek = Get_DayOfWeek_Formula(ui8DayOfMonth,ui8Month,ui8Year);
    
    Serial1.print("DOW: ");
    Serial1.println(ui8DayOfWeek);

    if(ui8Year > 0){
        Write();
    }
}

String ESPRTC::Get_DateTime_String(void){
    String sTime;
    if(ui8Hour < 10){
        sTime = "0" + String(ui8Hour) + ":";
    }else{
        sTime = String(ui8Hour) + ":";
    }

    if(ui8Minute < 10){
        sTime = sTime + "0" + String(ui8Minute) + ":";
    }else{
        sTime = sTime + String(ui8Minute) + ":";
    }

    if(ui8Second < 10){
        sTime = sTime + "0" + String(ui8Second) + " ";
    }else{
        sTime = sTime + String(ui8Second) + " ";
    }

    if(ui8DayOfMonth < 10){
        sTime = sTime + "0" + String(ui8DayOfMonth) + "-";
    }else{
        sTime = sTime + String(ui8DayOfMonth) + "-";
    }

    if(ui8Month < 10){
        sTime = sTime + "0" + String(ui8Month) + "-";
    }else{
        sTime = sTime + String(ui8Month) + "-";
    }

    if(ui8Year < 10){
        sTime = sTime + "0" + String(ui8Year);
    }else{
        sTime = sTime + String(ui8Year);
    }

    return sTime;
    
}

uint8_t ESPRTC::Get_dayOfWeek_Index(uint8_t ui8Week){
    uint8_t ui8Index=0;

    if(ui8Week == 0){
        ui8Index = 1;
    }

    if(ui8Week == 1){
        ui8Index = 2;
    }

    if(ui8Week == 2){
        ui8Index = 4;
    }

    if(ui8Week == 3){
        ui8Index = 8;
    }

    if(ui8Week == 4){
        ui8Index = 16;
    }

    if(ui8Week == 5){
        ui8Index = 32;
    }

    if(ui8Week == 6){
        ui8Index = 64;
    }

    return ui8Index;
    
}

uint8_t ESPRTC::Get_DayOfWeek_Formula(uint8_t day,uint8_t mth,uint8_t yr){
    uint8_t ui8Value;
    const int8 table[12]= {6,2,2,5,0,3,5,1,4,6,2,4};
    //// yr is last two digits Ex 2007 is 07
    ///  mth is month Jan=1.....Dec=12
    ui8Value=yr+yr/4; /// leap year adj good 2007 to 2099
    ui8Value=ui8Value+table[mth-1]; /// table contains modulo 7  adjustments for mths
    ui8Value=ui8Value+day; /// actual day 1..31
    if((yr%4==0) &&(mth<3))ui8Value=ui8Value-1; /// adjust jan and feb down one for leap year
    ui8Value=ui8Value%7;  /// modulo 7
    if(ui8Value==0)ui8Value=7;

    Serial1.println(daysOfTheWeek[ui8Value]);

    /// val is now the day of week 1=Mon .......7=Sun
    return(ui8Value);

}

// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
ESPRTC ERTC;
#endif