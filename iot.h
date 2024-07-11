bool WiFi_IsConnected = false;
bool IOT__Trying = false;



void Mqtt__Connect() {
	// if(_Mqtt_TryConnecting) return;
	// _Mqtt_TryConnecting = false;
	try {
		if (!_MqttObj.connected() || _MqttObj.state() < 0) {
			if (_MqttObj.connect(_MqttClientId.c_str(), _MqttUser.c_str(), _MqttPass.c_str())) {
				_MqttCon_IsConnected = true;
				if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mqtt_Connect Success Status :: " + String(_MqttObj.state()));
				_MqttObj.subscribe((_ProjectID + "/dvc").c_str());
				_MqttObj.subscribe((_ProjectID + "/appdvc").c_str());
				_MqttObj.subscribe((_ProjectID + "/common").c_str());
				//_MqttObj.subscribe("ZSLS00010001/#");
			} else {
				_MqttCon_IsConnected = false;
				if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mqtt_Connect Fail Status :: " + String(_MqttObj.state()));
			}
		}
	} catch (const std::exception &ex) {
		if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mqtt_Connect Error :: " + String(ex.what()));
	}
	//_Mqtt_TryConnecting = false;
}



void Mqtt__Stop() {
	/*
    if (_MqttCon_IsConnected) {
        _MqttCon_IsConnected = false;
        try {
            _MqttObj.disconnect();
        } catch (const std::exception &ex) {
            if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mqtt__Stop Err: _MqttObj.disconnect()");
        }
        delay(200);
        // try {
        //     _MqttObj = PubSubClient(wifiClient);
        // } catch (const std::exception &ex) {
        //   if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mqtt__Stop Err: _MqttObj = PubSubClient(wifiClient)");
        // }
        _MqttCon_Steps = 0;
    }
    _MqttCon_IsConnected = false;
	*/
}


bool _MqttRcv_IsWorking = false;

void Mqtt__OnRecieve(char *topic, uint8_t *payload, unsigned int length) {
	// if (_MqttRcv_IsWorking) return;
	// _MqttRcv_IsWorking = true;
	try {
		if (SOFTWARE_DEBUGMODE) {
			Debug_ConsolePrint("Mqtt Rcv... ");
			Debug_ConsolePrintln(topic);
		}
		delay(30);
		char *cleanPayload = (char *)malloc(length + 1);
		memcpy(cleanPayload, payload, length);
		cleanPayload[length] = '\0';
		String msg = String(cleanPayload);
		free(cleanPayload);

		//............................................... Data Converting (if JSON)
		Tools__DataToJSON(_DataRcv_JSON, msg);
		//............................................... Data Proccessing By (Scope & Type)
		if (_Started_IotRun && !CA_Equal(_DataRcv_JSON.Sender, _MySenderID)) {
			if (CA_Equal(_DataRcv_JSON.Scope, "appdvc")) {
				if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln(msg);
				if (CA_Equal(_DataRcv_JSON.Type, "modbus")) {
					if (_Need_Extender) {
						Tools__ModbusSend(CA_ToString(_DataRcv_JSON.Data), "Wire");  // Modbus Command Share On Wire
						delay(70);
					}
					Dvc__AnalyzeData(CA_ToString(_DataRcv_JSON.Data), DVC_CIRCUIT_WIFI_2);  // Modbus Command Check For This device (type => 2:wifi / 1:wire)
				}
			}
		}
	} catch (const std::exception &ex) {}
	//delay(30);
	//_MqttRcv_IsWorking = false;
}



void Mqtt__Send(String scope, String type, String data, String sender) {
	try {
		//................................. Reject If Mesh Not Connected
		if (!_MqttCon_IsConnected) return;
		if (!_MqttObj.connected() || _MqttObj.state() < 0) return;

		//................................. Checking Data
		scope.toLowerCase();
		String topic = _ProjectID + "/" + scope;

		char scopeArray[scope.length() + 1];
		CA_CopyStr(scopeArray, scope);

		char dataArray[data.length() + 1];
		CA_CopyStr(dataArray, data);

		char typeArray[type.length() + 1];
		CA_CopyStr(typeArray, type);

		String msg = Tools__DataToString(scopeArray, typeArray, dataArray, _MySenderID);
		if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mqtt Send... " + topic + "  >  " + msg);
		_MqttObj.publish(topic.c_str(), msg.c_str());
	} catch (const std::exception &ex) {
		if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("MqttSend Error: " + String(ex.what()));
	}
}





void IOT__WiFiStart() {
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Connecting to Wi-Fi...");
	Tools__WifiPower(true);
	const char *ssid = _ModemSSID.c_str();
	const char *password = _ModemPass.c_str();
	WiFi.begin(ssid, password);
	//Dvc__SetPinout();
}


void IOT__WiFiEvent(WiFiEvent_t event) {
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("[WiFi-event] event: " + String(event));
	switch (event) {

		case SYSTEM_EVENT_STA_GOT_IP:
			if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("WiFi connected, IP: " + WiFi.localIP().toString());
			WiFi_IsConnected = true;
			_MqttCon_Steps = 2;
			break;

		case SYSTEM_EVENT_STA_DISCONNECTED:
			WiFi_IsConnected = false;
			_MqttCon_IsConnected = false;
			_MqttCon_Steps = 0;
			if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("WiFi lost connection");
			break;
	}
	delay(100);
}



void IOT__Checker() {
	if (IOT__Trying) return;
	IOT__Trying = true;
	//......................................
	try {
		if (_MqttCon_Steps == 0) {
			IOT__WiFiStart();
			if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("############ _MqttCon_Steps 0 :: WiFiStart()");
			_MqttCon_Steps = 1;
		} else if (_MqttCon_Steps == 2) {
			const char *mqtt_broker = _MqttBroker.c_str();
			const int mqtt_port = 1883;  //5000
			_MqttObj.setCallback(Mqtt__OnRecieve);
			_MqttObj.setServer(mqtt_broker, mqtt_port);
			_MqttCon_Steps = 3;
			if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("############ _MqttCon_Steps 1 :: MqttObj.setServer()");
			//......................................
		} else if (_MqttCon_Steps == 3) {
			Mqtt__Connect();
		}
	} catch (const std::exception &ex) {}
	//......................................
	IOT__Trying = false;
}

Task IOT__Checker_Task(TASK_SECOND * 1, TASK_FOREVER, &IOT__Checker);



void IOT__Loop() {
	while (true) {
		try {
			if (_MqttCon_IsConnected) {
				_MqttObj.loop();
				delay(50);
			}
		} catch (const std::exception &ex) {
			if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("IOT__Loop Error: " + String(ex.what()));
		}
		delay(TASK_LOOP_DELAY);
	}
}



void IOT__Setup(void *param) {
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln(".... IOT Mqtt Setup ....");
	//....................................... Reject if Wifi-SSID empty
	if (_ModemSSID == "") {
		Tools__SetMode("normal", true);
		return;
	}

	//........................................... WiFi Settings
	if (WiFi.isConnected()) {
		WiFi.disconnect(true);
		delay(100);
	}
	WiFi.onEvent(IOT__WiFiEvent);
	WiFi_IsConnected = false;

	//............................................ Mqtt Settings
	_MqttClientId = _ProjectID + "d" + _SerialNo + "0000" + String(Tools__Random(100000, 999999));  // WiFi.macAddress()
	_MqttClientId += "xxxxxxxxxxxxxxxxxxxxxxx";
	_MqttClientId = _MqttClientId.substring(0, 23);
	_MqttCon_IsConnected = false;
	_MqttCon_Steps = 0;
	IOT__Trying = false;
	_TaskRunner.addTask(IOT__Checker_Task);
	IOT__Checker_Task.enableIfNot();
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln(".... IOT Setup OK ...");

	//........................................... Stay In Loop
	IOT__Loop();
}
