#ifdef __cplusplus
extern "C" {
#endif
#include "lib\freertos\include\freertos\FreeRTOS.h"
#include "lib\freertos\include\freertos\timers.h"
	uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

#include "Arduino.h"
#include "lib/eeprom/EEPROM2.cpp"
#include <WString.h>
#include <HardwareSerial.h>
#include "lib\wifi\WiFi.h" 
#include <WiFiClient.h>
#include <WebServer.h>  // "lib\webserver\WebServer.cpp"
#include "lib\mesh\painlessMesh.h"
#include <PubSubClient.h>
#include "lib\dns\ESPmDNS.cpp"
#include <Adafruit_NeoPixel.h>  // "lib\neo\Adafruit_NeoPixel.cpp"

#include "definitions.h"
#include "common.h"
#include "config.h"
#include "mesh.h"
#include "sendall.h"
#include "iot.h"
#include "dvcbutton.h"
#include "dvc.h"





void TaskTimerLoop(void* param) {
	while (true) {
		try {
			_TaskRunner.execute();
		} catch (const std::exception& ex) {
			if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Error TaskTimerLoop: " + String(ex.what()));
		}
		delay(TASK_LOOP_DELAY);
	}
}



void Setting_ForTest() {
	//------------- change for test only --------------------
	_Extender = 0;
	_Circuit = "iot";
	_ProjectID = "ZSLS00010001";  //""
	_EncryptionKey = "";

	_ModemSSID = "Zoorilco";       //TP-LINK_BE2C74"; "";
	_ModemPass = "mohammad0937A";  //84719980";       "";
	_ModemChannel = 1;

	_MeshChannel = 1;
	_MeshChannelOld = 1;
	_MeshName = "m1";
	_MeshPass = "mpass0000";

	_MqttBroker = "192.168.1.200";
	_MqttUser = "zooril";
	_MqttPass = "zooril0000";
	//--------------------------------------------------------
}




void setup() {
	//delay(2000);
	//Dvc__SetPinout();
	Button__LEDShow();  //Set LED On or Off
	//...................................... 3 Sec Delay to Stable Power
	delay(3000);

	//...................................... Preparing EPROM
	//  if (SOFTWARE_DEBUGMODE) {
	Serial.setRxBufferSize(1024);
	Serial.begin(9600);
	Serial.flush();
	//  }

	//.................
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("\r\n=========================\r\n======= Restarted =======\r\n=========================");


	if (!EEPROM.begin(EEPROM_SIZE)) {
		_Fail = 0;
		while (!EEPROM.begin(EEPROM_SIZE)) {
			if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("EEPROM Failed to init: ");
			if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln(String(_Fail));
			delay(2000);
			if (++_Fail > 5) {
				Tools__Reboot();
				return;
			}
		}
	}
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("EEPROM OK");

	//...................................... Create Token For This Session
	if (_Token == "") _Token = (String(Tools__GetChipID()) + "0000000000").substring(0, 10) + String(Tools__Random(100000, 999998));  // 16 Digit Token


	/******************************************************** /
                     Test FOr DEBUG Only
    /********************************************************/
	// _TestRebootCount = MemoReadByte(EP_TEST_REBOOT_COUNT);
	//if (_TestRebootCount >= 255) _TestRebootCount = 0;
	// _TestRebootCount++;
	// MemoWriteByte(EP_TEST_REBOOT_COUNT, _TestRebootCount);
	//if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("\r\n\r\nReboot Count: " + String(_TestRebootCount) + "\r\n\r\n");
	/********************************************************/

	//...................................... load settings
	Tools__SettingDefault();
	Tools__SettingRead();

	Tools__DvcAddressLoad();
	Tools__ScenarioLoad();
	Tools__RunsenLoad();

	//...................................... Serial Barcode Correction
	//Serial.println("_SerialNo");
	//MemoWriteString(EP_SERIAL_S,EP_SERIAL_E,"229ZZZBBBA");
	//_SerialNo = MemoReadString(EP_SERIAL_S,EP_SERIAL_E);
	Serial.println("_SerialNo");
	Serial.println(_SerialNo.length());
	Serial.println(_SerialNo);
	if (_SerialNo.length() != 10) {
		_SerialNo = _SerialNo + "0000000000";
		_SerialNo = _SerialNo.substring(0, 10);
	}
	if (_SerialNo.substring(0, 3) != "000") {
		_Dvc_HasBarcode = true;
	} else {  // Build Random Serial
		String chip = _Token.substring(0, 7);
		if (chip == "0000000") chip = (String(Tools__Random(1000000, 9999998)) + "0000000").substring(0, 7);
		_SerialNo = ("000" + chip).substring(0, 10);
	}
	Serial.println(_SerialNo);
	CA_CopyStr(_MySenderID, "dvc_" + _SerialNo);

	//...................................................
	//  if (_Configured == 0) Setting_ForTest();  // ************ For Test Only ***
	//  Tools__SettingShowInfo();


	//-------------------------------------------------------- Preparing
	Tools__TaskSetup();
	if (_Circuit == "wired") {
		_Need_Wire = true;
		_Started_WiredOnly = true;
	}
	//........................
	else if (_Circuit == "wifi") {
		_Need_Mesh = true;
	} else if (_Circuit == "wirefi") {
		_Need_Wire = true;
		_Need_Mesh = true;
		if (_Extender == 1) _Need_Extender = true;
	}
	//........................
	else if (_Circuit == "iot") {
		_Need_IOT = true;
	} else if (_Circuit == "wiot") {
		_Need_IOT = true;
		_Need_Wire = true;
		if (_Extender == 1) _Need_Extender = true;
	}
	//........................
	if (_StartMode == "config_panel") {
		_Started_ConfigPanel = true;
		_Need_Taskloop = true;
	} else if (_StartMode == "reboot_sendall") {
		_Started_RebootSendall = true;
	} else if (_StartMode == "config_sendall") {
		_Started_ConfigSendall = true;
	} else if (_StartMode == "config_ranking") {
		_Started_ConfigRanking = true;
	} else {  // Started_Normal
		if (!_Started_WiredOnly) {
			if (_Need_IOT) {
				_Started_IotRun = true;
			} else if (_Need_Mesh) {
				_Started_MeshRun = true;
			}
		}
		if (!_Started_MeshRun) _Need_Taskloop = true;
	}


	Manual__Setup();

	// _Started_ConfigPanel = true; //*************** FOR TEST ONLY


	//...................................... Set WiFi-Power
	Tools__WifiPower(_Started_WiredOnly ? false : true);

	//----------------------------------------------------------------- Running By Mode (On XTask-Thread)
	//...................................... Loop ShdulerTimerTask
	if (_Need_Taskloop) xTaskCreatePinnedToCore(TaskTimerLoop, "TaskTimerLoop_Task", TASK_STACKSIZE_LARGE, NULL, TASK_PRIORITY_MID, NULL, 1);


	//...................................... Setup LED & TouchKey (To Use Manualy)

	//  if (!_Started_ConfigPanel && !_Started_ConfigSendall && !_Started_RebootSendall && !_Started_IotRun && !_Started_MeshRun) {
	//    Manual__Setup(NULL);
	//  } else {
	//    xTaskCreatePinnedToCore(Manual__Setup, "Manual__Setup_Task", TASK_STACKSIZE_LARGE, NULL, TASK_PRIORITY_MID, NULL, 1);
	//  }

	//----------------------------------------------------------------- Running By Mode (On Main Thread)
	//...................................... Setup On Mode-Config
	if (_Started_ConfigPanel) {
		Config__Setup(NULL);
	}
	//...................................... Setup On Send Setting/Send RootRank/Send Reboot To All (On Old Mesh)
	else if (_Started_ConfigSendall || _Started_RebootSendall) {
		//    Sendall__Setup(NULL);
	}
	//............................................. Setup On Mode IOT / Mqtt
	else if (_Started_IotRun) {
		//    IOT__Setup(NULL);
	}
	//............................................. Setup On Mode Wifi-Mesh
	else if (_Started_MeshRun) {
		//    Mesh__Setup(NULL);
	}
}



void loop() {

	delay(100);
}
