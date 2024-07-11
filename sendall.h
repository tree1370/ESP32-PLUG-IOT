byte SendallCount = 0;
byte SendallCountDisconnect = 0;
String SendallData = "";



void Sendall__Setup(void *param) {
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln(".... SendAll Setup ....");
	//...................................... Set Powercut Restart-Point (ConfigSendall)
	if (_Started_ConfigSendall) {
		Tools__Memory_StrSet("_StartMode", "normal");
		if (_MeshNameOld == "") {
			Tools__Reboot_Task.enableIfNot();
			return;
		}
		Mesh__PrepareWithTasks(_MeshNameOld, _MeshPassOld, _MeshChannelOld, false);
		// Mesh__Connect();

		//....................................... ConfigSendall : nedds to setting-data correction before send
		//.................. ConfigSendall: Make data JSON-string
		String data = "{";
		data += "'circuit':'" + _Circuit + "',";
		data += "'projectId':'" + _ProjectID + "',";
		data += "'encryptionKey':'" + _EncryptionKey + "',";

		data += "'meshName':'" + _MeshName + "',";
		data += "'meshPass':'" + _MeshPass + "',";
		data += "'meshChannel':'" + String(_MeshChannel) + "',";

		data += "'modemChannel':" + String(_ModemChannel) + ",";
		data += "'modemSsid':'" + _ModemSSID + "',";
		data += "'modemPass':'" + _ModemPass + "',";

		data += "'mqttUser':'" + _MqttUser + "',";
		data += "'mqttPass':'" + _MqttPass + "',";
		data += "'mqttBroker':'" + _MqttBroker + "',";
		data += "}";
		char dataArray[data.length() + 1];
		CA_CopyStr(dataArray, data);
		SendallData = Tools__DataToString("dvc", "setting", dataArray, "");
	}
	//...................................... Set Powercut Restart-Point (RebootSendall)
	else if (_Started_RebootSendall) {
		Tools__Memory_StrSet("_StartMode", "normal");
		Mesh__PrepareWithTasks(_MeshName, _MeshPass, _MeshChannel, false);
		// Mesh__Connect();
	}

	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln(".... SendAll Setup OK");

	//.......................................
	_MeshNameOld = "";
	_MeshPassOld = "";
	Tools__Memory_StrSet("_MeshNameOld", "");
	Tools__Memory_StrSet("_MeshPassOld", "");

	//....................................... Run Task to Send all Several Times
	_TaskRunner.addTask(Sendall__ShareData_Task);
	Sendall__ShareData_Task.enableIfNot();
	Sendall__ShareData_Task.forceNextIteration();

	//........................................... Stay In Loop
	Mesh__Loop(); // Sendall__Loop()
}


/*
void Sendall__Loop() {
	while (true) {
		try {
			Mesh__Loop();
		} catch (const std::exception &ex) {
			if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Sendall__Loop Error: " + String(ex.what()));
		}
		delay(TASK_LOOP_DELAY);
	}
}
*/


void Sendall__ShareData() {  //Send Data On WifiMesh
	SendallCountDisconnect++;
	if (SendallCountDisconnect > 6) {
		SendallCount = 10;
	}
	//...................................... Reject If Mesh Not Connected
	else {
		if (!_Mesh_IsConnected) return;
		else SendallCountDisconnect = 0;
	}

	//....................................... RebootSendall : Send RebootCommand to all
	if (_Started_RebootSendall) {
		if (++SendallCount <= 2) {
			Mesh__Send("share", "reboot");
		} else {
			Sendall__ShareData_Task.disable();
			Tools__SetMode("normal", true);
		}
	}
	//....................................... ConfigSendall : Send Settings to all
	else if (_Started_ConfigSendall) {
		if (++SendallCount <= 6) {
			Mesh__Send("setting", SendallData);
		} else {
			Sendall__ShareData_Task.disable();
			Tools__SetMode("normal", true);
		}
	}
}
