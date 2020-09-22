/*******************************************************************************
*File Name: ESPRTC.h
*
* Version: 1.0
*
* Description:
* In this header used for managing RTC
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , SwitchBinary
*******************************************************************************/
#ifndef ESPRTC_h
#define ESPRTC_h
#include <Arduino.h>

#define DS3231_I2C_ADDRESS 0x68

#define RTC_NONE        0
#define RTC_START_ERROR 1
#define RTC_START_OK    2
#define RTC_RUN_OK      3
#define RTC_RUN_ERROR   4

class ESPRTC{
public:
    ESPRTC();
    uint8_t ui8RTCStatus;
    uint8_t ui8Year;
    uint8_t ui8Month;
    uint8_t ui8DayOfMonth;
    uint8_t ui8Hour;
    uint8_t ui8Minute;
    uint8_t ui8Second;
    uint8_t ui8DayOfWeek;
    uint8_t ui8StartFlag=1;


    //char daysOfTheWeek[7][12] = {"Thursday", "Friday", "Saturday","Sunday", "Monday", "Tuesday", "Wednesday"};
    
    char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday","Thursday", "Friday", "Saturday"};

    void Start(void);
    String Read(void);
    void Write(void);
    void handleTiming(void);
    byte DecToBcd(byte val);
    byte BcdToDec(byte val);
    uint8_t isrunning(void);
    void Update_Time_NTP(void);
    String Get_DateTime_String(void);
    uint8_t Get_dayOfWeek_Index(uint8_t ui8Week);
    uint8_t Get_DayOfWeek_Formula(uint8_t day, uint8_t mth,uint8_t yr);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern ESPRTC ERTC;
#endif
#endif