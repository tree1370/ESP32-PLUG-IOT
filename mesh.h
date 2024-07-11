String MeshParam_Name = "";
String MeshParam_Pass = "";
byte MeshParam_Channel = 1;
bool MeshParam_IsRoot = false;

byte MeshCon_NeedReset = 0;
byte MeshCon_Missed = 0;
byte MeshCon_RootMissed = 0;
bool MeshCon_Trying = false;

bool _MeshRcv_IsWorking = false;


void Mesh__OnReceive(const uint32_t& from, const String& msg) {
	// if (_MeshRcv_IsWorking) return;
	// _MeshRcv_IsWorking = true;
	try {
		if (SOFTWARE_DEBUGMODE) Debug_ConsolePrint("Mesh Received: ");
		if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln(msg.c_str());
		Tools__DataToJSON(_DataRcv_JSON, msg);
		if (CA_Equal(_DataRcv_JSON.Scope, "appdvc") || CA_Equal(_DataRcv_JSON.Scope, "dvc")) {
			//============================== if not itself
			if (!CA_Equal(_DataRcv_JSON.Sender, _MySenderID)) {
				//.......................... modbus
				if (CA_Equal(_DataRcv_JSON.Type, "modbus")) {
					if (_Need_Extender) {
						Tools__ModbusSend(CA_ToString(_DataRcv_JSON.Data), "Wire");
						delay(70);
					}
					Dvc__AnalyzeData(CA_ToString(_DataRcv_JSON.Data), DVC_CIRCUIT_WIFI_2);
				}
				//.......................... alive
				else if (CA_Equal(_DataRcv_JSON.Type, "alive")) {
					//MeshCon_RootMissed = 0;
				}
				//.......................... setting
				else if (CA_Equal(_DataRcv_JSON.Type, "setting")) {  //save new setting received from mesh
					String json = CA_ToString(_DataRcv_JSON.Data);
					if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("JSON WAS : " + json);
					json.replace("'", "\"");
					if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("JSON IS : " + json);
					Tools__SettingFromJSON(json);
					Tools__SettingSave();
					delay(500);
					Tools__SetMode("normal", true);
				}
				//.......................... share (reboot, ....)
				if (CA_Equal(_DataRcv_JSON.Type, "share")) {
					if (CA_Equal(_DataRcv_JSON.Data, "reboot")) Tools__Reboot_Task.enableIfNot();
				}
			}
			//============================== include itself
			else {
			}
		}
	} catch (const std::exception& ex) {}
	// delay(30);
	// _MeshRcv_IsWorking = false;
}



void Mesh__OnNewConnection(uint32_t nodeId) {
	_Mesh_IsConnected = true;
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("*************** Mesh New Connection, nodeId = " + String(nodeId));
}



void Mesh__OnChangedConnection() {
	_Mesh_NodesList = _MeshObj.getNodeList();
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("*************** Mesh Changed connections\nNodesList: " + String(_Mesh_NodesList.size()) + " / Wifi Status: " + String(WiFi.status()));
	if (_Mesh_NodesList.size() > 0) {
		_Mesh_IsConnected = true;
	} else {
		_Mesh_IsConnected = false;
		if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mesh Missid, need to rescan");
		Mesh__StopToReconnect();
	}
}



void Mesh__SendByTask() {  //Send Data On WifiMesh
	//................................. Using Data & Clean
	Mesh__SendTask.disable();
	char data[_Mesh_SendData.length() + 1];
	CA_CopyStr(data, _Mesh_SendData);
	char type[_Mesh_SendType.length() + 1];
	CA_CopyStr(type, _Mesh_SendType);
	_Mesh_SendData[0] = '\0';
	_Mesh_SendType[0] = '\0';
	//.................................
	try {
		if (data != "") {
			String jsonData = "";
			if (type == "modbus") {
				jsonData = Tools__DataToString("dvcapp", "modbus", data, "");
				_MeshObj.sendBroadcast(jsonData, false);  // false to broadcast to all but itself
				if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mesh Send : " + jsonData);
			} else if (type == "alive" || type == "setting") {
				jsonData = data;                          // data already is json-package, no neet to convert
				_MeshObj.sendBroadcast(jsonData, false);  // false to broadcast to all but itself
				if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mesh Send : " + jsonData);
			} else if (type == "share") {
				jsonData = Tools__DataToString("dvc", "share", data, "");
				_MeshObj.sendBroadcast(jsonData, true);  // false to broadcast to all and itself
				if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mesh Send (Share): " + jsonData);
			} else {
				if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mesh Data-Type Is Wrong (" + CA_ToString(type) + ") : " + CA_ToString(data));
			}
		} else {
			if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mesh Data Is Null");
		}
	} catch (const std::exception& ex) {
		if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("MeshSendTask Error: " + String(ex.what()));
	}
}



void Mesh__Send(String type, String data) {  //Send Data On WifiMesh
	try {
		//................................. Reject If Mesh Not Connected
		if (!_Mesh_IsConnected) return;
		//.................................
		if (data != "") {
			if (type == "modbus" || type == "alive" || type == "setting" || type == "share") {
				_Mesh_SendData = data + "";
				_Mesh_SendType = type + "";
				Mesh__SendTask.enableIfNot();
				Mesh__SendTask.forceNextIteration();
			}
		} else {
			if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mesh No Broadcasting Due To Null-Data");
		}
	} catch (const std::exception& ex) {
		if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("MeshSend Error: " + String(ex.what()));
	}
}



void Mesh__Connect() {
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("########## Mesh__Connecting...");
	//.......................................... Reconnect-Params Set
	Mesh__Connect_Task.disable();
	_Mesh_IsConnected = false;
	MeshCon_Missed = 0;
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mesh__Connect To [" + MeshParam_Name + " / " + MeshParam_Pass + "] On Channel " + String(MeshParam_Channel));
	try {
		Tools__WifiPower(true);
		WiFi.mode(WIFI_AP_STA);
		//WiFi.forceSleepWake();
		//.......................................... Mesh Connecting
		if (SOFTWARE_DEBUGMODE) _MeshObj.setDebugMsgTypes(ERROR | STARTUP | CONNECTION | MESH_STATUS | SYNC | GENERAL);  // set before init() so that you can see startup messages
		else _MeshObj.setDebugMsgTypes(0);                                                                               // set before init() so that you can see startup messages
		String meshName = _ProjectID + "_" + MeshParam_Name;
		char mN[meshName.length()];
		meshName.toCharArray(mN, meshName.length() + 1);
		char mP[MeshParam_Pass.length()];
		MeshParam_Pass.toCharArray(mP, MeshParam_Pass.length() + 1);
		_MeshObj.init(mN, mP, &_TaskRunner, MESH__PORT, WIFI_AP_STA, MeshParam_Channel, MESH__NOHIDE);  //MESH__HIDDEN
		_MeshObj.onReceive(&Mesh__OnReceive);
		_MeshObj.onNewConnection(&Mesh__OnNewConnection);
		_MeshObj.onChangedConnections(&Mesh__OnChangedConnection);
		_MeshObj.setRoot(MeshParam_IsRoot);
		_MeshObj.setContainsRoot(true);
	} catch (const std::exception& ex) {
		if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mesh__Connect Error: " + String(ex.what()));
	}
	//..........................................
	MeshCon_Trying = false;
}



void Mesh__StopToReconnect() {
	if (MeshCon_Trying) return;
	//...................................... Mesh stop
	MeshCon_Trying = true;
//	Dvc__LastStatusSave();
	// Tools__FreeMemory_Task.enableIfNot();
	// Tools__FreeMemory_Task.forceNextIteration();
	_TaskRunner.execute();

	if (_Mesh_IsConnected) {
		_Mesh_IsConnected = false;
		try {
			_MeshObj.stop();
		} catch (const std::exception& ex) {
		}
		delay(200);
	}
	try {
		WiFi.mode(WIFI_OFF);
		WiFi.disconnect();
		delay(1);
	} catch (const std::exception& ex) {}
	delay(100);
	//...................................... Mesh reconnect
	Mesh__Connect_Task.enableIfNot();
}



void Mesh__CheckRootAlive() {
	//..................................... Checking Mesh (For All MeshType)
	_Mesh_NodesList = _MeshObj.getNodeList();
	if (_Mesh_NodesList.size() <= 0) {
		_Mesh_IsConnected = false;
		if (++MeshCon_Missed >= 2) {
			if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("\n******************* Check Mesh\n");
			if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("NodesList: " + String(_Mesh_NodesList.size()) + " / Wifi Status: " + String(WiFi.status()));
			MeshCon_NeedReset += 2;
			if (MeshCon_NeedReset > MESH__MISSTIMEOUT) {
				//................................................. Need Restart ESP (on current mode again)
				Tools__SetMode("normal", true);
			}
			//..................................................... Need To Reconnect Mesh
			else {
				Mesh__StopToReconnect();
			}
		}
	} else {
		_Mesh_IsConnected = true;
		MeshCon_Missed = 0;
		MeshCon_NeedReset = 0;
	}
	//..................................... Return If Sendall
	// if (_Started_IotRun || _Started_ConfigSendall) return;

	//..................................... Sending RootAlive (For Root MeshType)
	// if (_Started_MRootRun__) {
	//     Mesh__Send("alive", _Mqtt_RootAliveData);
	// }
	//..................................... Checking RootAlive (For Regular MeshType)
	// else if (_MeshTyp_e != "fix_regular") {
	//     if (MeshCon_RootMissed < NODE__MISSTIMEOUT) {
	//         MeshCon_RootMissed++;
	//         if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Alive Root Is Not Valid : " + String(MeshCon_RootMissed));
	//     }
	//     //.....................................
	//     else {  // Need to find new node after NODE__MISSTIMEOUT, due to missing Root (10 * 30sec = 5min)
	//         MeshCon_RootMissed = 0;
	//         if (_MeshTyp_e == "fix_reserve" || _MeshTyp_e == "auto_regular") {
	//             Tools__SetMode("meshroot_ranking", true);
	//         }
	//     }
	// }
}
Task Mesh__CheckRootAlive_Task(TASK_SECOND * 30, TASK_FOREVER, &Mesh__CheckRootAlive);



void Mesh__PrepareWithTasks(String mname = "", String mpass = "", byte channel = 1, bool isroot = false) {
	if (channel > 14 || channel <= 0) channel = 1;
	MeshParam_Name = mname;
	MeshParam_Pass = mpass;
	MeshParam_Channel = channel;
	MeshParam_IsRoot = isroot;
	//...................................
	_Mesh_IsConnected = false;
	MeshCon_Missed = 0;

	//...................................
	_TaskRunner.addTask(Mesh__Connect_Task);
	Mesh__Connect_Task.disable();

	_TaskRunner.addTask(Mesh__SendTask);
	Mesh__SendTask.disable();

	_TaskRunner.addTask(Mesh__CheckRootAlive_Task);
	Mesh__CheckRootAlive_Task.enable();
	Mesh__Connect();
}



void Mesh__Loop() {
	while (true) {
		try {
			_MeshObj.update();
			delay(50);
		} catch (const std::exception& ex) {
			if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mesh__Loop Error: " + String(ex.what()));
		}
		delay(TASK_LOOP_DELAY);
	}
}



void Mesh__Setup(void* param) {
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln(".... Mesh Setup ....");
	//........................................... Set Powercut Restart-Point
	Tools__Memory_StrSet("_StartMode", "normal");

	//........................................... Prepare Mesh
	Mesh__PrepareWithTasks(_MeshName, _MeshPass, _MeshChannel, _Need_Extender);
	// Mesh__Connect();
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln(".... Mesh Setup OK");

	//........................................... Stay In Loop
	Mesh__Loop();
}
