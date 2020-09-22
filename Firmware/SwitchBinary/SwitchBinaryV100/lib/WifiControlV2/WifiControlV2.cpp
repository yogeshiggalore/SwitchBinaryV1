/*******************************************************************************
*File Name: WifiControlV2.c
*
* Version: 1.0
*
* Description:
* In this source code for wifi connection related functions
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , SwitchBinary
*******************************************************************************/
#include <WifiControlV2.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ESPUtils.h>
#include <ESP8266WiFiMulti.h>
#include <ESPFile.h>
#include <Application.h>

// Constructors ////////////////////////////////////////////////////////////////
WifiControlV2::WifiControlV2(void):wifiMulti(){}

void WifiControlV2::Start(void){
	uint8_t ui8WifiCounter=0;
	Update_All_Wifi_Add_File();
	Get_All_Wifi_Add_Parameter();
	Update_Host_Name();

	wifiMulti.addAP(sAddWifiName1.c_str(), sAddWifiPassword1.c_str());
	wifiMulti.addAP(sAddWifiName2.c_str(), sAddWifiPassword2.c_str());
	wifiMulti.addAP(sAddWifiName3.c_str(), sAddWifiPassword3.c_str());
	wifiMulti.addAP(sAddWifiName4.c_str(), sAddWifiPassword4.c_str());
	wifiMulti.addAP("silicosmos", "test1234");
	
	for(ui8WifiCounter=0;ui8WifiCounter<30;ui8WifiCounter++){
		if(wifiMulti.run() != WL_CONNECTED){
			delay(200);
			digitalWrite(PIN_YELLOW_LED,!digitalRead(PIN_YELLOW_LED));
		}else{
			ui8WifiCounter = 181;
			Serial1.print("Connected to wifi ");
			Serial1.println(WiFi.localIP());
			CWifiIPAddr = WiFi.localIP();
			CWifiGateway = WiFi.gatewayIP();
			CWifiSubnet = WiFi.subnetMask();
			CWifiDNS    = WiFi.dnsIP();
			CSSID = WiFi.SSID();
			sCurrentSSID = WiFi.SSID();
			Check_Wifi_Connetivity();
			EApp.Application_Pin_Write(YELLOW_LED,HIGH);
		}
	}
}

uint8_t WifiControlV2::Get_Status(void){
	return ui8WifiStatus;
}

void WifiControlV2::Set_Status(uint8_t ui8Status){
	ui8WifiStatus = ui8Status;
}

String WifiControlV2::Checkout_Connection(void){
	Set_Status(wifiMulti.run());
	String sString;
	if(Get_Status() != WL_CONNECTED){
		if(Get_Status() == WL_NO_SHIELD){
			sString = "\r\nwifi error:WL_NO_SHIELD";
		}else if(Get_Status() == WL_IDLE_STATUS){
			sString = "\r\nwifi error:WL_IDLE_STATUS";
		}else if(Get_Status() == WL_NO_SSID_AVAIL){
			sString = "\r\nwifi error:WL_NO_SSID_AVAIL";
		}else if(Get_Status() == WL_SCAN_COMPLETED){
			sString = "\r\nwifi error:WL_SCAN_COMPLETED";
		}else if(Get_Status() == WL_CONNECT_FAILED){
			sString = "\r\nwifi error:WL_CONNECT_FAILED";
		}else if(Get_Status() == WL_CONNECTION_LOST){
			sString = "\r\nwifi error:WL_CONNECTION_LOST";
		}else if(Get_Status() == WL_DISCONNECTED){
			sString = "\r\nwifi error:WL_DISCONNECTED";
		}else{
			sString = "Something else";
		}
	}else{
		sString = "\r\nConnected to wifi ";
		
	}
	
	if(ui8WifiStartFlag == 0){
		ui16BootWifiCounter++;
		if(ui16BootWifiCounter > BOOT_WIFI_COUNT_MAX){
			ui16BootWifiCounter = 0;
			ui8WifiStartFlag = 1;
		}
	}
	
	return sString;
}

uint8_t WifiControlV2::Update_Wifi_Add_Static_Config(uint8_t ui8FileIndex,uint8_t ui8UpdateIndex, String sUpdateString){
  String sWifiParamter;
  String  sSubString;
  String sStoredIP;
  String sStoredGateway;
  String sStoredSubnet;
  String sStoredDNS;
  String   sStroredConfigState;
  uint8_t  ui8CommaIndex;
  uint8_t  ui8LastCommaIndex;
  uint8_t  aui8ConfigArray[4];
  String sWriteData;
  IPAddress ConfigArray;

  sWifiParamter = EFile.Read_Data_From_File(sESPFiles[WIFI_1_PARAMETER_INDEX + ui8FileIndex]);
  if(sWifiParamter == "NOWIFI"){
	sWifiParamter = "0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,";
  }
	ui8CommaIndex = sWifiParamter.indexOf(',');
	ui8LastCommaIndex = 0;
		if(ui8CommaIndex > 0){
			sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
			Serial1.print("ConfigValue: ");
			Serial1.println(sSubString);
			sStroredConfigState = sSubString;
			ui8LastCommaIndex = ui8CommaIndex + 1;
			ui8CommaIndex = sWifiParamter.indexOf(',',ui8LastCommaIndex);
			if(ui8CommaIndex > 0){
				sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
				Serial1.print("IP: ");
				Serial1.println(sSubString);
				sStoredIP = sSubString;
				ui8LastCommaIndex = ui8CommaIndex + 1;
				ui8CommaIndex = sWifiParamter.indexOf(',',ui8LastCommaIndex);
				if(ui8CommaIndex > 0){
					sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
					Serial1.print("Gateway: ");
					Serial1.println(sSubString);
					sStoredGateway  = sSubString;
					ui8LastCommaIndex = ui8CommaIndex + 1;
					ui8CommaIndex = sWifiParamter.indexOf(',',ui8LastCommaIndex);
					if(ui8CommaIndex > 0){
						sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
						Serial1.print("Subnet: ");
						Serial1.println(sSubString);
						sStoredSubnet = sSubString;
						ui8LastCommaIndex = ui8CommaIndex + 1;
						ui8CommaIndex = sWifiParamter.indexOf(',',ui8LastCommaIndex);
						if(ui8CommaIndex > 0){
							sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
							Serial1.print("DNS: ");
							Serial1.println(sSubString);
							sStoredDNS = sSubString;
							ui8LastCommaIndex = ui8CommaIndex + 1;
							ui8CommaIndex = sWifiParamter.indexOf(',',ui8LastCommaIndex);
							if(ui8CommaIndex > 0){
							  sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
							Serial1.print("configarray: ");
							Serial1.println(sSubString);
							EUtils.Parse_String_To_Bytes(sSubString.c_str(),'.',aui8ConfigArray,4,10);
							ConfigArray[0] = aui8ConfigArray[0];
							ConfigArray[1] = aui8ConfigArray[1];
							ConfigArray[2] = aui8ConfigArray[2];
							ConfigArray[3] = aui8ConfigArray[3];
							}
						}
					}
				}
			}
		}
	
	if(ui8UpdateIndex == 1){
		if(sUpdateString.toInt() == 1){
			sStroredConfigState = "1";
		}else{
			sStroredConfigState = "0";
		}
	}

	if(ui8UpdateIndex == 2){
		sStoredIP = sUpdateString;
		if(sUpdateString == "0.0.0.0"){
			aui8ConfigArray[0] = 0;
			ConfigArray[0] = 0;
		}else{
			aui8ConfigArray[0] = 1;
			ConfigArray[0] = 1;
		}
	}

	if(ui8UpdateIndex == 3){
		sStoredGateway = sUpdateString;
		if(sUpdateString == "0.0.0.0"){
			aui8ConfigArray[1] = 0;
			ConfigArray[1] = 0;
		}else{
			aui8ConfigArray[1] = 1;
			ConfigArray[1] = 1;
		}
	}

	if(ui8UpdateIndex == 4){
		sStoredSubnet = sUpdateString;
		if(sUpdateString == "0.0.0.0"){
			aui8ConfigArray[2] = 0;
			ConfigArray[2] = 0;
		}else{
			aui8ConfigArray[2] = 1;
			ConfigArray[2] = 1;
		}
	}

	if(ui8UpdateIndex == 5){
		sStoredDNS = sUpdateString;
		if(sUpdateString == "0.0.0.0"){
			aui8ConfigArray[3] = 0;
			ConfigArray[3] = 0;
		}else{
			aui8ConfigArray[3] = 1;
			ConfigArray[3] = 1;
		}
	}

	sWriteData = sStroredConfigState + "," + sStoredIP + "," + sStoredGateway + "," + sStoredSubnet + "," + sStoredDNS + "," + ConfigArray.toString() + "," ; 
	Serial1.println("wifi add Write data");
	Serial1.println(sWriteData);

	if(EFile.DeleteFile(sESPFiles[WIFI_1_PARAMETER_INDEX + ui8FileIndex],&EFile.aui32ESPFileSize[WIFI_1_PARAMETER_INDEX + ui8FileIndex]) == FILE_DELETE_SUCCESS){
		if(EFile.Write_Data_To_File(sESPFiles[WIFI_1_PARAMETER_INDEX + ui8FileIndex],sWriteData) == FILE_STORE_SUCCESS){
			return 1;
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}

void WifiControlV2::Check_Wifi_Connetivity(void){
	sCurrentSSID = WiFi.SSID();
	if(sCurrentSSID != sLastSSID){
		sLastSSID = sCurrentSSID;
		Serial1.println("Connected to new wifi:");
		Serial1.println(sCurrentSSID);
		if(sCurrentSSID == sAddWifiName1){
			Check_Wifi_Add_Parameters(0);
		}

		if(sCurrentSSID == sAddWifiName2){
			Check_Wifi_Add_Parameters(1);
		}

		if(sCurrentSSID == sAddWifiName3){
			Check_Wifi_Add_Parameters(2);
		}

		if(sCurrentSSID == sAddWifiName4){
			Check_Wifi_Add_Parameters(3);
		}
	}
}

uint8_t WifiControlV2::Check_Wifi_Add_Parameters(uint8_t ui8FileIndex){
  String sWifiParamter;
  String  sSubString;
  IPAddress IPAddre;
  IPAddress GatewayAddr;
  IPAddress SubnetAddr;
  IPAddress DNSAddr;
  String   sStroredConfigState;
  uint8_t  ui8CommaIndex;
  uint8_t  ui8LastCommaIndex;
  uint8_t  aui8ConfigArray[4];
  String sWriteData;
  IPAddress ConfigArray;

  sWifiParamter = EFile.Read_Data_From_File(sESPFiles[WIFI_1_PARAMETER_INDEX + ui8FileIndex]);
  if(sWifiParamter == "NOWIFI"){
	sWifiParamter = "0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,";
  }else{
	ui8CommaIndex = sWifiParamter.indexOf(',');
	ui8LastCommaIndex = 0;
		if(ui8CommaIndex > 0){
			sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
			Serial1.print("ConfigValue: ");
			Serial1.println(sSubString);
			sStroredConfigState = sSubString;
			ui8LastCommaIndex = ui8CommaIndex + 1;
			ui8CommaIndex = sWifiParamter.indexOf(',',ui8LastCommaIndex);
			if(ui8CommaIndex > 0){
				sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
				Serial1.print("IP: ");
				Serial1.println(sSubString);
				EUtils.Parse_String_To_Bytes(sSubString.c_str(),'.',aui8ConfigArray,4,10);
				IPAddre[0] = aui8ConfigArray[0];
				IPAddre[1] = aui8ConfigArray[1];
				IPAddre[2] = aui8ConfigArray[2];
				IPAddre[3] = aui8ConfigArray[3];
				ui8LastCommaIndex = ui8CommaIndex + 1;
				ui8CommaIndex = sWifiParamter.indexOf(',',ui8LastCommaIndex);
				if(ui8CommaIndex > 0){
					sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
					Serial1.print("Gateway: ");
					Serial1.println(sSubString);
					EUtils.Parse_String_To_Bytes(sSubString.c_str(),'.',aui8ConfigArray,4,10);
					GatewayAddr[0] = aui8ConfigArray[0];
					GatewayAddr[1] = aui8ConfigArray[1];
					GatewayAddr[2] = aui8ConfigArray[2];
					GatewayAddr[3] = aui8ConfigArray[3];
					ui8LastCommaIndex = ui8CommaIndex + 1;
					ui8CommaIndex = sWifiParamter.indexOf(',',ui8LastCommaIndex);
					if(ui8CommaIndex > 0){
						sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
						Serial1.print("Subnet: ");
						Serial1.println(sSubString);
						EUtils.Parse_String_To_Bytes(sSubString.c_str(),'.',aui8ConfigArray,4,10);
						SubnetAddr[0] = aui8ConfigArray[0];
						SubnetAddr[1] = aui8ConfigArray[1];
						SubnetAddr[2] = aui8ConfigArray[2];
						SubnetAddr[3] = aui8ConfigArray[3];
						ui8LastCommaIndex = ui8CommaIndex + 1;
						ui8CommaIndex = sWifiParamter.indexOf(',',ui8LastCommaIndex);
						if(ui8CommaIndex > 0){
							sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
							Serial1.print("DNS: ");
							Serial1.println(sSubString);
							EUtils.Parse_String_To_Bytes(sSubString.c_str(),'.',aui8ConfigArray,4,10);
							DNSAddr[0] = aui8ConfigArray[0];
							DNSAddr[1] = aui8ConfigArray[1];
							DNSAddr[2] = aui8ConfigArray[2];
							DNSAddr[3] = aui8ConfigArray[3];
							ui8LastCommaIndex = ui8CommaIndex + 1;
							ui8CommaIndex = sWifiParamter.indexOf(',',ui8LastCommaIndex);
							if(ui8CommaIndex > 0){
							  sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
							Serial1.print("configarray: ");
							Serial1.println(sSubString);
							EUtils.Parse_String_To_Bytes(sSubString.c_str(),'.',aui8ConfigArray,4,10);
							ConfigArray[0] = aui8ConfigArray[0];
							ConfigArray[1] = aui8ConfigArray[1];
							ConfigArray[2] = aui8ConfigArray[2];
							ConfigArray[3] = aui8ConfigArray[3];
							}
						}
					}
				}
			}
		}

		if(sStroredConfigState == "1"){
			if(ConfigArray[0] == 0){
				IPAddre = WiFi.localIP();
			}

			if(ConfigArray[1] == 0){
				GatewayAddr = WiFi.gatewayIP();
			}

			if(ConfigArray[2] == 0){
				SubnetAddr = WiFi.subnetMask();
			}

			if(ConfigArray[3] == 0){
				DNSAddr = WiFi.dnsIP();
			}

			WiFi.config(IPAddre,GatewayAddr,SubnetAddr,DNSAddr);
		}
  }

  return 1;
}


void WifiControlV2::Convert_String_IP_Formate(String sWifiParamter, uint8_t* aui8IP, uint8_t* aui8Gateway, uint8_t* aui8Subnet, uint8_t* aui8DNS, uint8_t* aui8Config){
  String  sSubString;
  uint8_t  ui8CommaIndex;
  uint8_t  ui8LastCommaIndex;
  uint8_t  aui8ConfigArray[4];

	ui8CommaIndex = sWifiParamter.indexOf(',');
	ui8LastCommaIndex = 0;
		if(ui8CommaIndex > 0){
			sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
			Serial1.print("ConfigValue: ");
			Serial1.println(sSubString);
			if(sSubString == "1"){
				aui8Config[4] = 1;
			}else{
				aui8Config[4] = 0;
			}
			ui8LastCommaIndex = ui8CommaIndex + 1;
			ui8CommaIndex = sWifiParamter.indexOf(',',ui8LastCommaIndex);
			if(ui8CommaIndex > 0){
				sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
				Serial1.print("IP: ");
				Serial1.println(sSubString);
				EUtils.Parse_String_To_Bytes(sSubString.c_str(),'.',aui8ConfigArray,4,10);
				aui8IP[0] = aui8ConfigArray[0];
				aui8IP[1] = aui8ConfigArray[1];
				aui8IP[2] = aui8ConfigArray[2];
				aui8IP[3] = aui8ConfigArray[3];
				ui8LastCommaIndex = ui8CommaIndex + 1;
				ui8CommaIndex = sWifiParamter.indexOf(',',ui8LastCommaIndex);
				if(ui8CommaIndex > 0){
					sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
					Serial1.print("Gateway: ");
					Serial1.println(sSubString);
					EUtils.Parse_String_To_Bytes(sSubString.c_str(),'.',aui8ConfigArray,4,10);
					aui8Gateway[0] = aui8ConfigArray[0];
					aui8Gateway[1] = aui8ConfigArray[1];
					aui8Gateway[2] = aui8ConfigArray[2];
					aui8Gateway[3] = aui8ConfigArray[3];
					ui8LastCommaIndex = ui8CommaIndex + 1;
					ui8CommaIndex = sWifiParamter.indexOf(',',ui8LastCommaIndex);
					if(ui8CommaIndex > 0){
						sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
						Serial1.print("Subnet: ");
						Serial1.println(sSubString);
						EUtils.Parse_String_To_Bytes(sSubString.c_str(),'.',aui8ConfigArray,4,10);
						aui8Subnet[0] = aui8ConfigArray[0];
						aui8Subnet[1] = aui8ConfigArray[1];
						aui8Subnet[2] = aui8ConfigArray[2];
						aui8Subnet[3] = aui8ConfigArray[3];
						ui8LastCommaIndex = ui8CommaIndex + 1;
						ui8CommaIndex = sWifiParamter.indexOf(',',ui8LastCommaIndex);
						if(ui8CommaIndex > 0){
							sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
							Serial1.print("DNS: ");
							Serial1.println(sSubString);
							EUtils.Parse_String_To_Bytes(sSubString.c_str(),'.',aui8ConfigArray,4,10);
							aui8DNS[0] = aui8ConfigArray[0];
							aui8DNS[1] = aui8ConfigArray[1];
							aui8DNS[2] = aui8ConfigArray[2];
							aui8DNS[3] = aui8ConfigArray[3];
							ui8LastCommaIndex = ui8CommaIndex + 1;
							ui8CommaIndex = sWifiParamter.indexOf(',',ui8LastCommaIndex);
							if(ui8CommaIndex > 0){
							  sSubString = sWifiParamter.substring(ui8LastCommaIndex, ui8CommaIndex);
							Serial1.print("configarray: ");
							Serial1.println(sSubString);
							EUtils.Parse_String_To_Bytes(sSubString.c_str(),'.',aui8ConfigArray,4,10);
							aui8Config[0] = aui8ConfigArray[0];
							aui8Config[1] = aui8ConfigArray[1];
							aui8Config[2] = aui8ConfigArray[2];
							aui8Config[3] = aui8ConfigArray[3];
							}
						}
					}
				}
			}
		}
}

void WifiControlV2::Get_All_Wifi_Add_Parameter(void){
	Get_Wifi_Add_Parameter(0);
	Get_Wifi_Add_Parameter(1);
	Get_Wifi_Add_Parameter(2);
	Get_Wifi_Add_Parameter(3);
}

void WifiControlV2::Get_Wifi_Add_Parameter(uint8_t ui8FileIndex){
	String sWifiParamter;
	uint8_t aui8IP[4];
	uint8_t aui8Gateway[4];
	uint8_t aui8Subnet[4];
	uint8_t aui8DNS[4];
	uint8_t aui8Config[5]; 

	if(ui8FileIndex == 0){
		sWifiParamter = EFile.Read_Data_From_File(sESPFiles[WIFI_1_PARAMETER_INDEX + ui8FileIndex]);
		if(sWifiParamter != "NOWIFI"){
			Convert_String_IP_Formate(sWifiParamter,aui8IP,aui8Gateway,aui8Subnet,aui8DNS,aui8Config);
			AddWifiIP1[0] = aui8IP[0];
			AddWifiIP1[1] = aui8IP[1];
			AddWifiIP1[2] = aui8IP[2];
			AddWifiIP1[3] = aui8IP[3];
			AddWifiGateway1[0] = aui8Gateway[0];
			AddWifiGateway1[1] = aui8Gateway[1];
			AddWifiGateway1[2] = aui8Gateway[2];
			AddWifiGateway1[3] = aui8Gateway[3];
			AddWifiSubnet1[0] = aui8Subnet[0];
			AddWifiSubnet1[1] = aui8Subnet[1];
			AddWifiSubnet1[2] = aui8Subnet[2];
			AddWifiSubnet1[3] = aui8Subnet[3];
			AddWifiDNS1[0] = aui8DNS[0];
			AddWifiDNS1[1] = aui8DNS[1];
			AddWifiDNS1[2] = aui8DNS[2];
			AddWifiDNS1[3] = aui8DNS[3];
			AddWifiConfig1[0] = aui8Config[0];
			AddWifiConfig1[1] = aui8Config[1];
			AddWifiConfig1[2] = aui8Config[2];
			AddWifiConfig1[3] = aui8Config[3];
			ui8AddWifiConfigEnable1 = aui8Config[4];
		}
	}

	if(ui8FileIndex == 1){
		sWifiParamter = EFile.Read_Data_From_File(sESPFiles[WIFI_1_PARAMETER_INDEX + ui8FileIndex]);
		if(sWifiParamter != "NOWIFI"){
			Convert_String_IP_Formate(sWifiParamter,aui8IP,aui8Gateway,aui8Subnet,aui8DNS,aui8Config);
			AddWifiIP2[0] = aui8IP[0];
			AddWifiIP2[1] = aui8IP[1];
			AddWifiIP2[2] = aui8IP[2];
			AddWifiIP2[3] = aui8IP[3];
			AddWifiGateway2[0] = aui8Gateway[0];
			AddWifiGateway2[1] = aui8Gateway[1];
			AddWifiGateway2[2] = aui8Gateway[2];
			AddWifiGateway2[3] = aui8Gateway[3];
			AddWifiSubnet2[0] = aui8Subnet[0];
			AddWifiSubnet2[1] = aui8Subnet[1];
			AddWifiSubnet2[2] = aui8Subnet[2];
			AddWifiSubnet2[3] = aui8Subnet[3];
			AddWifiDNS2[0] = aui8DNS[0];
			AddWifiDNS2[1] = aui8DNS[1];
			AddWifiDNS2[2] = aui8DNS[2];
			AddWifiDNS2[3] = aui8DNS[3];
			AddWifiConfig2[0] = aui8Config[0];
			AddWifiConfig2[1] = aui8Config[1];
			AddWifiConfig2[2] = aui8Config[2];
			AddWifiConfig2[3] = aui8Config[3];
			ui8AddWifiConfigEnable2 = aui8Config[4];
		}
	}

	if(ui8FileIndex == 2){
		sWifiParamter = EFile.Read_Data_From_File(sESPFiles[WIFI_1_PARAMETER_INDEX + ui8FileIndex]);
		if(sWifiParamter != "NOWIFI"){
			Convert_String_IP_Formate(sWifiParamter,aui8IP,aui8Gateway,aui8Subnet,aui8DNS,aui8Config);
			AddWifiIP3[0] = aui8IP[0];
			AddWifiIP3[1] = aui8IP[1];
			AddWifiIP3[2] = aui8IP[2];
			AddWifiIP3[3] = aui8IP[3];
			AddWifiGateway3[0] = aui8Gateway[0];
			AddWifiGateway3[1] = aui8Gateway[1];
			AddWifiGateway3[2] = aui8Gateway[2];
			AddWifiGateway3[3] = aui8Gateway[3];
			AddWifiSubnet3[0] = aui8Subnet[0];
			AddWifiSubnet3[1] = aui8Subnet[1];
			AddWifiSubnet3[2] = aui8Subnet[2];
			AddWifiSubnet3[3] = aui8Subnet[3];
			AddWifiDNS3[0] = aui8DNS[0];
			AddWifiDNS3[1] = aui8DNS[1];
			AddWifiDNS3[2] = aui8DNS[2];
			AddWifiDNS3[3] = aui8DNS[3];
			AddWifiConfig3[0] = aui8Config[0];
			AddWifiConfig3[1] = aui8Config[1];
			AddWifiConfig3[2] = aui8Config[2];
			AddWifiConfig3[3] = aui8Config[3];
			ui8AddWifiConfigEnable3 = aui8Config[4];
		}
	}

	if(ui8FileIndex == 3){
		sWifiParamter = EFile.Read_Data_From_File(sESPFiles[WIFI_1_PARAMETER_INDEX + ui8FileIndex]);
		if(sWifiParamter != "NOWIFI"){
			Convert_String_IP_Formate(sWifiParamter,aui8IP,aui8Gateway,aui8Subnet,aui8DNS,aui8Config);
			AddWifiIP4[0] = aui8IP[0];
			AddWifiIP4[1] = aui8IP[1];
			AddWifiIP4[2] = aui8IP[2];
			AddWifiIP4[3] = aui8IP[3];
			AddWifiGateway4[0] = aui8Gateway[0];
			AddWifiGateway4[1] = aui8Gateway[1];
			AddWifiGateway4[2] = aui8Gateway[2];
			AddWifiGateway4[3] = aui8Gateway[3];
			AddWifiSubnet4[0] = aui8Subnet[0];
			AddWifiSubnet4[1] = aui8Subnet[1];
			AddWifiSubnet4[2] = aui8Subnet[2];
			AddWifiSubnet4[3] = aui8Subnet[3];
			AddWifiDNS4[0] = aui8DNS[0];
			AddWifiDNS4[1] = aui8DNS[1];
			AddWifiDNS4[2] = aui8DNS[2];
			AddWifiDNS4[3] = aui8DNS[3];
			AddWifiConfig4[0] = aui8Config[0];
			AddWifiConfig4[1] = aui8Config[1];
			AddWifiConfig4[2] = aui8Config[2];
			AddWifiConfig4[3] = aui8Config[3];
			ui8AddWifiConfigEnable4 = aui8Config[4];
		}
	}

}	

void WifiControlV2::Update_All_Wifi_Add_File(void){
  sAddWifiName1 = EFile.Read_Data_From_File(sESPFiles[WIFI_1_SSID_INDEX]);
  sAddWifiPassword1 = EFile.Read_Data_From_File(sESPFiles[WIFI_1_PASSWORD_INDEX]);

  sAddWifiName2 = EFile.Read_Data_From_File(sESPFiles[WIFI_2_SSID_INDEX]);
  sAddWifiPassword2 = EFile.Read_Data_From_File(sESPFiles[WIFI_2_PASSWORD_INDEX]);

  sAddWifiName3 = EFile.Read_Data_From_File(sESPFiles[WIFI_3_SSID_INDEX]);
  sAddWifiPassword3 = EFile.Read_Data_From_File(sESPFiles[WIFI_3_PASSWORD_INDEX]);

  sAddWifiName4 = EFile.Read_Data_From_File(sESPFiles[WIFI_4_SSID_INDEX]);
  sAddWifiPassword4 = EFile.Read_Data_From_File(sESPFiles[WIFI_4_PASSWORD_INDEX]);

}

void WifiControlV2::Update_Wifi_Add_File(uint8_t ui8FileId){
	if(ui8FileId == 0){
		sAddWifiName1 = EFile.Read_Data_From_File(sESPFiles[WIFI_1_SSID_INDEX]);
		sAddWifiPassword1 = EFile.Read_Data_From_File(sESPFiles[WIFI_1_PASSWORD_INDEX]);
	}

  	if(ui8FileId == 1){
	  sAddWifiName2 = EFile.Read_Data_From_File(sESPFiles[WIFI_2_SSID_INDEX]);
	  sAddWifiPassword2 = EFile.Read_Data_From_File(sESPFiles[WIFI_2_PASSWORD_INDEX]);
	}

  	if(ui8FileId == 2){
		sAddWifiName3 = EFile.Read_Data_From_File(sESPFiles[WIFI_3_SSID_INDEX]);
		sAddWifiPassword3 = EFile.Read_Data_From_File(sESPFiles[WIFI_3_PASSWORD_INDEX]);
	}

  	if(ui8FileId == 3){
		sAddWifiName4 = EFile.Read_Data_From_File(sESPFiles[WIFI_4_SSID_INDEX]);
		sAddWifiPassword4 = EFile.Read_Data_From_File(sESPFiles[WIFI_4_PASSWORD_INDEX]);
	}
}

void WifiControlV2::Update_Host_Name(void){
	sHostName = EFile.Read_Data_From_File(sESPFiles[WIFI_HOSTNAME_INDEX]);
	sHostPassword = EFile.Read_Data_From_File(sESPFiles[HOST_PASSWORD_INDEX]);
}

// Preinstantiate Objects //////////////////////////////////////////////////////

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
WifiControlV2 EWifiControl;
#endif
