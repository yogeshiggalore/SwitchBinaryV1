/*******************************************************************************
*File Name: WifiControlV2.h
*
* Version: 1.0
*
* Description:
* In this header used for wifi connection related functions functions
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , SwitchBinary
*******************************************************************************/
#ifndef WifiControlV2_h
#define WifiControlV2_h
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#define BOOT_WIFI_COUNT_MAX 180

#define WIFI_USE_CONFIG_PARA_INDEX  0
#define WIFI_USE_IP_ADDR_INDEX      1
#define WIFI_USE_GATWAY_ADDR_INDEX  2
#define WIFI_USE_SUBNET_ADDR_INDEX  3
#define WIFI_USE_DNS_ADDR_INDEX     4

class WifiControlV2{
public:
	uint8_t  ui8WifiStatus=0;
	uint16_t ui16BootWifiCounter=0;
	uint8_t  ui8WifiStartFlag=0;

	IPAddress CWifiIPAddr;
	IPAddress CWifiGateway;
	IPAddress CWifiSubnet;
	IPAddress CWifiDNS;

	String CSSID;
	String CPassword;

	String sAddWifiName1;
	String sAddWifiName2;
	String sAddWifiName3;
	String sAddWifiName4;

	String sAddWifiPassword1;
	String sAddWifiPassword2;
	String sAddWifiPassword3;
	String sAddWifiPassword4;

	String sHostName;
	String sHostPassword;
	
	IPAddress AddWifiIP1;
	IPAddress AddWifiIP2;
	IPAddress AddWifiIP3;
	IPAddress AddWifiIP4;

	IPAddress AddWifiGateway1;
	IPAddress AddWifiGateway2;
	IPAddress AddWifiGateway3;
	IPAddress AddWifiGateway4;

	IPAddress AddWifiSubnet1;
	IPAddress AddWifiSubnet2;
	IPAddress AddWifiSubnet3;
	IPAddress AddWifiSubnet4;

	IPAddress AddWifiDNS1;
	IPAddress AddWifiDNS2;
	IPAddress AddWifiDNS3;
	IPAddress AddWifiDNS4;

	IPAddress AddWifiConfig1;
	IPAddress AddWifiConfig2;
	IPAddress AddWifiConfig3;
	IPAddress AddWifiConfig4;

	uint8_t  ui8AddWifiConfigEnable1;
	uint8_t  ui8AddWifiConfigEnable2;
	uint8_t  ui8AddWifiConfigEnable3;
	uint8_t  ui8AddWifiConfigEnable4;

	String  sLastSSID;
	String  sCurrentSSID;
	uint8_t ui8StaticChangeFlag;

	uint16_t ui16NoWifiCounter=0;

	WifiControlV2();
	void Start(void);
	String Checkout_Connection(void);
	uint8_t Get_Status(void);
	void Set_Status(uint8_t ui8Status);
	uint8_t Check_Wifi_Parameters(void);
	uint8_t Check_Wifi_Add_Parameters(uint8_t ui8FileIndex);
	void Update_Wifi_Parameters(void);
	void Update_Wifi_ssid_password(void);
	void Update_All_Wifi_Add_File(void);
	void Update_Wifi_Add_File(uint8_t ui8FileId);
	void Update_Host_Name(void);
	void Get_All_Wifi_Add_Parameter(void);
	void Get_Wifi_Add_Parameter(uint8_t ui8FileIndex);
	uint8_t Update_Wifi_Add_Static_Config(uint8_t ui8FileIndex,uint8_t ui8UpdateIndex, String sUpdateString);
	void Convert_String_IP_Formate(String sWifiParamter, uint8_t* aui8IP, uint8_t* aui8Gateway, uint8_t* aui8Subnet, uint8_t* aui8DNS, uint8_t* aui8Config);
	void Check_Wifi_Connetivity(void);
	uint8_t Verify_For_Static_Config_Enable(void);
	void Test_WifiConfigParameters(void);
	void Test_WifiConfigStatus(void);
	
private:
	ESP8266WiFiMulti wifiMulti;
};
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern WifiControlV2 EWifiControl;
#endif
#endif
