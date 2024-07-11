void Debug_ConsolePrint(String p1) {
	if (!SOFTWARE_DEBUGMODE) return;
	try {
		Serial.print(p1);
	} catch (const std::exception& ex) {
		Serial.println("");
		Serial.println("Err Debug_ConsolePrint");
		Serial.println("");
	}
}


void Debug_ConsolePrintln(String p1) {
	if (!SOFTWARE_DEBUGMODE) return;
	try {
		Serial.println(p1);
		Serial.flush();
	} catch (const std::exception& ex) {
		Serial.println("");
		Serial.println("Err Debug_ConsolePrintln");
		Serial.println("");
	}
}





int Tools__StringSplitSize(String inp, char splitter) {
	int j, i = 0, max = inp.length();
	for (j = 0; j < max; j++) {
		if (inp.charAt(j) == splitter) i++;
	}
	i++;
	return i;
}


void Tools__StringSplit(String inp, char splitter, String outArray[], int sizeOfArray) {
	int j, j_old = 0, i = 0, max = inp.length();
	for (j = 0; j < max && i < sizeOfArray; j++) {
		if (inp.charAt(j) == splitter) {
			outArray[i] = inp.substring(j_old, j);
			j_old = j + 1;
			i++;
		}
	}
	if (i < sizeOfArray) outArray[i] = inp.substring(j_old);
}


byte Tools__StringToByte(String inp) {
	byte r = 0;
	try {
		r = (byte)((inp).toInt() & 0xFF);
	} catch (const std::exception& ex) {}
	return r;
}



String Tools__ByteToHexstring(long b) {
	String Str = String(b, HEX);
	Str.toUpperCase();
	return (Str.length() % 2 == 0 ? "" : "0") + Str;
}



uint64_t Tools__HexToUInt64(String p) {
	p.toUpperCase();
	uint64_t accumulator = 0;
	for (size_t i = 0; isxdigit((unsigned char)p[i]); ++i) {
		char c = p[i];
		accumulator *= 16;
		if (isdigit(c)) accumulator += c - '0';  // '0' .. '9'
		else accumulator += c - 'A' + 10;        //'A'  .. 'F'
	}
	return accumulator;
}



long Tools__HexToDecimal(String value) {
	int base = 16;
	int length = value.length() + 1;
	char valueAsArray[length];
	value.toCharArray(valueAsArray, length);
	return strtol(valueAsArray, NULL, base);
}


String Tools__HexStrToString(String value) {
	String res = "";
	value.trim();
	int len = value.length();
	if (len % 2 != 0) {
		value = "0" + value;
		len++;
	}
	for (int i = 0; i < len; i += 2) {
		res += (char)(Tools__HexToUInt64(value.substring(i, i + 2)) & 0xFF);
	}
	return res;
}


String Tools__StringToHexStr(String value) {
	String res = "";
	int len = value.length();
	for (int i = 0; i < len; i++) {
		res += Tools__ByteToHexstring((byte)value.charAt(i));
	}
	return res;
}



byte Tools__StringToCharVal(String inp, byte index) {
	if (inp == "") return 0;
	char c = inp.charAt(index);
	return (byte)c;
}






void CA_ConcateStr(char output[], String str, int max = 0) {
	int j, k, c, i = strlen(output), len = str.length();
	for (j = 0; j < len; j++) {
		k = i + j;
		c = str.charAt(j);
		if (c == '\0' || (max > 0 && k >= max)) break;
		output[k] = c;
	}
	output[i + j] = '\0';
}


void CA_Concate(char output[], char input[], int max = 0) {
	int i = strlen(output), j, k, len = strlen(input);
	for (j = 0; j < len; j++) {
		k = i + j;
		if (input[j] == '\0' || (max > 0 && k >= max)) break;
		output[k] = input[j];
	}
	output[i + j] = '\0';
}


void CA_Copy(char output[], char input[], int max = 0) {
	int i, len = strlen(input);
	for (i = 0; i < len; i++) {
		if (max > 0 && i >= max) break;
		if (input[i] == '\0') break;
		output[i] = input[i];
	}
	output[i] = '\0';
}


void CA_CopyStr(char output[], String str, int max = 0) {
	int i, c, len = str.length();
	for (i = 0; i < len; i++) {
		if (max > 0 && i >= max) break;
		c = str.charAt(i);
		if (c == '\0') break;
		output[i] = c;
	}
	output[i] = '\0';
}


void CA_Sub(char input[], int start = 0, int len = 0) {
	int i, j, end = strlen(input);
	if (len == 0) len = end - start;
	if (len <= 0) {
		input[0] = '\0';
	} else {
		char tmp[len + 1];
		for (i = start, j = 0; j < len; i++, j++) {
			if (input[i] == '\0') break;
			tmp[j] = input[i];
		}
		tmp[j] = '\0';
		for (i = 0; i <= j; i++) input[i] = tmp[i];
	}
}


int CA_IndexOf(char input[], char srch[]) {
	int i, j, max = strlen(input), len = strlen(srch);
	if (len > max || len == 0) return -1;
	max = max - len;
	for (i = 0; i <= max; i++) {
		for (j = 0; j < len; j++) {
			if (input[i + j] != srch[j]) break;
		}
		if (j == len) return i;
	}
	return -1;
}


int CA_Compare(char input1[], char input2[]) {
	int i, v, min, len1 = strlen(input1), len2 = strlen(input2);
	min = len1;
	if (len2 < len1) min = len2;
	for (i = 0; i < min; i++) {
		v = ((byte)input1[i]) - ((byte)input2[i]);
		if (v != 0) break;
	}
	if (v == 0) return (len1 - len2);
	else return v;
}



bool CA_Equal(char input1[], char input2[]) {
	int i, v, min, len1 = strlen(input1), len2 = strlen(input2);
	if (len1 != len2) return false;
	min = len1;
	if (len2 < len1) min = len2;
	for (i = 0; i < min; i++) {
		v = ((byte)input1[i]) - ((byte)input2[i]);
		if (v != 0) return false;
	}
	return true;
}


bool CA_StartWith(char input[], char srch[]) {
	int i, len = strlen(srch);
	if (len > strlen(input)) return false;
	for (i = 0; i < len; i++) {
		if (input[i] != srch[i]) return false;
	}
	return true;
}


void CA_UpperCase(char input[]) {
	int i, v, len = strlen(input);
	for (i = 0; i < len; i++) {
		v = (byte)input[i];
		if (v >= 97 && v <= 122) input[i] = ((char)(v - 32));
	}
}


void CA_LowerCase(char input[]) {
	int i, v, len = strlen(input);
	for (i = 0; i < len; i++) {
		v = (byte)input[i];
		if (v >= 65 && v <= 90) input[i] = ((char)(v + 32));
	}
}



void CA_RightPad(char input[], int max, char c = ' ') {
	try {
		int i, v, len = strlen(input);
		for (i = len; i < max; i++) input[i] = c;
	} catch (const std::exception& ex) {}
}


void CA_Trim(char inp[]) {
	int start = 0;
	char* buffer = inp;
	while (*inp && *inp++ == ' ') ++start;
	while (*inp++)
		;
	int end = inp - buffer - 1;
	while (end > 0 && buffer[end - 1] == ' ') --end;
	buffer[end] = 0;
	if (end <= start || start == 0) return;
	inp = buffer + start;
	while ((*buffer++ = *inp++))
		;
}


String CA_ToString(char input[], int start = 0, int len = 0) {
	int i, end = strlen(input);
	String str = "";
	if (len == 0) len = end - start;
	end = start + len;
	for (i = start; i < end; i++) {
		if (input[i] == '\0') break;
		str += input[i];
	}
	return str;
}



void MemoClean(int p_start = EP_MEMORYSTART, int p_end = EEPROM_SIZE) {
	for (int i = p_start; i <= p_end; i++) {
		EEPROM.write(i, 0xFF);
		EEPROM.commit();
	}
	delay(100);
}


void MemoWriteByte(int p_start, byte val) {
	EEPROM.write(p_start, val);
	EEPROM.commit();
	delay(10);
}


void MemoWriteString(int p_start, int p_end, String val) {
	int max = val.length();
	byte v[max];
	val.getBytes(v, max + 1);
	for (int i = p_start, c = 0; i <= p_end; i++, c++) {
		EEPROM.write(i, c < max ? v[c] : 0);
	}
	EEPROM.commit();
	delay(10);
}


int MemoReadByte(int p_start) {
	int r = 0;
	r = 0 + EEPROM.read(p_start);
	return r;
}


String MemoReadString(int p_start, int p_end) {
	String val = "";
	for (int i = p_start; i <= p_end; i++) {
		char v = EEPROM.read(i);
		if (v == '\0') break;
		val += ("" + String(v));
	}
	return val;
}


bool Tools__Memory_StrSet(String key, String val) {
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Tools__Memory_StrSet > " + key + " : " + val);
	try {
		if (key == "_Configured") {
			MemoWriteByte(EP_CONFIGURED, val == "1" ? 1 : 0);
		} else if (key == "_StartMode") {
			MemoWriteByte(EP_STARTMODE,
			              val == "normal"           ? 0
			              : val == "reboot_sendall" ? 7
			              : val == "config_sendall" ? 8
			              : val == "config_panel"   ? 9
			                                        : 0);
		} else if (key == "_Extender") {
			MemoWriteByte(EP_EXTENDER, val == "1" ? 1 : 0);
		} else if (key == "_SerialNo") {
			val.toUpperCase();
			MemoWriteString(EP_SERIAL_S, EP_SERIAL_E, val);
		} else if (key == "_Circuit") {
			MemoWriteByte(EP_CIRCUIT, val == "wired"    ? 1
			                          : val == "wifi"   ? 2
			                          : val == "wirefi" ? 3
			                          : val == "wiot"   ? 8
			                          : val == "iot"    ? 9
			                                            : 1);
		} else if (key == "_ProjectID") {
			val.toUpperCase();
			MemoWriteString(EP_PROJECTID_S, EP_PROJECTID_E, val);
		} else if (key == "_EncryptionKey") {
			MemoWriteString(EP_ENCRYPTIONKEY_S, EP_ENCRYPTIONKEY_E, val);
		} else if (key == "_MqttUser") {
			MemoWriteString(EP_MQTTUSER_S, EP_MQTTUSER_E, val);
		} else if (key == "_MqttPass") {
			MemoWriteString(EP_MQTTPASS_S, EP_MQTTPASS_E, val);
		} else if (key == "_MqttBroker") {
			MemoWriteString(EP_MQTTBROKER_S, EP_MQTTBROKER_E, val);
		} else if (key == "_MeshRSSI") {
			byte b = Tools__StringToByte(val);
			MemoWriteByte(EP_MESHRSSI, (b >= 0 && b < 255) ? b : 255);
		} else if (key == "_MeshName" || key == "_MeshNameOld") {
			if (key == "_MeshName") MemoWriteString(EP_MESHNAME_S, EP_MESHNAME_E, val);
			else MemoWriteString(EP_MESHNAME_OLD_S, EP_MESHNAME_OLD_E, val);
		} else if (key == "_MeshPass" || key == "_MeshPassOld") {
			if (key == "_MeshPass") MemoWriteString(EP_MESHPASS_S, EP_MESHPASS_E, val);
			else MemoWriteString(EP_MESHPASS_OLD_S, EP_MESHPASS_OLD_E, val);
		} else if (key == "_MeshChannel" || key == "_MeshChannelOld") {
			byte b = Tools__StringToByte(val);
			b = (b <= 14 && b > 0 ? b : 0);
			MemoWriteByte(key == "_MeshChannel" ? EP_MESHCHANNEL : EP_MESHCHANNEL_OLD, b);
		} else if (key == "_MqttBroker") {
			MemoWriteString(EP_MQTTBROKER_S, EP_MQTTBROKER_E, val);
		} else if (key == "_MqttUser") {
			MemoWriteString(EP_MQTTUSER_S, EP_MQTTUSER_E, val);
		} else if (key == "_MqttPass") {
			MemoWriteString(EP_MQTTPASS_S, EP_MQTTPASS_E, val);
		} else if (key == "_ModemChannel") {
			byte b = Tools__StringToByte(val);
			b = (b <= 14 && b > 0 ? b : 0);
			MemoWriteByte(EP_MODEMCHANNEL, b);
		} else if (key == "_ModemSSID") {
			MemoWriteString(EP_MODEMSSID_S, EP_MODEMSSID_E, val);
		} else if (key == "_ModemPass") {
			MemoWriteString(EP_MODEMPASS_S, EP_MODEMPASS_E, val);
		} else if (key == "_ModemRSSI") {
			byte b = Tools__StringToByte(val);
			b = (b < 255 && b >= 0 ? b : 255);
			MemoWriteByte(EP_MODEMRSSI, b);
		}
		return true;
	} catch (const std::exception& ex) {}
	return false;
}


String Tools__Memory_StrGet(String key) {
	bool checkNull = false;
	String res = "";
	int v = 0;
	try {
		if (key == "_Configured") {
			v = MemoReadByte(EP_CONFIGURED);
			res = (v == 1 ? "1" : "0");
		} else if (key == "_StartMode") {
			v = MemoReadByte(EP_STARTMODE);
			res = (v == 0   ? "normal"
			       : v == 7 ? "reboot_sendall"
			       : v == 8 ? "config_sendall"
			       : v == 9 ? "config_panel"
			                : "normal");
		} else if (key == "_Extender") {
			v = MemoReadByte(EP_EXTENDER);
			res = (v == 1 ? "1" : "0");
		} else if (key == "_SerialNo") {
			res = MemoReadString(EP_SERIAL_S, EP_SERIAL_E);
			if (res == "" || res[0] == char(255)) res = "0000000000";
		} else if (key == "_Circuit") {
			v = MemoReadByte(EP_CIRCUIT);
			res = (v == 1   ? "wired"
			       : v == 2 ? "wifi"
			       : v == 3 ? "wirefi"
			       : v == 8 ? "wiot"
			       : v == 9 ? "iot"
			                : "wired");
		} else if (key == "_ProjectID") {
			res = MemoReadString(EP_PROJECTID_S, EP_PROJECTID_E);
			res.toUpperCase();
			checkNull = true;
		} else if (key == "_EncryptionKey") {
			res = MemoReadString(EP_ENCRYPTIONKEY_S, EP_ENCRYPTIONKEY_E);
			checkNull = true;
		} else if (key == "_MqttUser") {
			res = MemoReadString(EP_MQTTUSER_S, EP_MQTTUSER_E);
			checkNull = true;
		} else if (key == "_MqttPass") {
			res = MemoReadString(EP_MQTTPASS_S, EP_MQTTPASS_E);
			checkNull = true;
		} else if (key == "_MqttBroker") {
			res = MemoReadString(EP_MQTTBROKER_S, EP_MQTTBROKER_E);
			checkNull = true;
		} else if (key == "_MeshRSSI") {
			v = MemoReadByte(EP_MESHRSSI);
			res = "" + String(v);
		} else if (key == "_MeshName" || key == "_MeshNameOld") {
			res = (key == "_MeshName" ? MemoReadString(EP_MESHNAME_S, EP_MESHNAME_E) : MemoReadString(EP_MESHNAME_OLD_S, EP_MESHNAME_OLD_E));
			checkNull = true;
		} else if (key == "_MeshPass" || key == "_MeshPassOld") {
			res = (key == "_MeshPass" ? MemoReadString(EP_MESHPASS_S, EP_MESHPASS_E) : MemoReadString(EP_MESHPASS_OLD_S, EP_MESHPASS_OLD_E));
			checkNull = true;
		} else if (key == "_MeshChannel" || key == "_MeshChannelOld") {
			v = MemoReadByte(key == "_MeshChannel" ? EP_MESHCHANNEL : EP_MESHCHANNEL_OLD);
			res = "" + String(v);
		} else if (key == "_ModemChannel") {
			v = MemoReadByte(EP_MODEMCHANNEL);
			res = "" + String(v);
		} else if (key == "_ModemSSID") {
			res = MemoReadString(EP_MODEMSSID_S, EP_MODEMSSID_E);
			checkNull = true;
		} else if (key == "_ModemPass") {
			res = MemoReadString(EP_MODEMPASS_S, EP_MODEMPASS_E);
			checkNull = true;
		} else if (key == "_ModemRSSI") {
			v = MemoReadByte(EP_MODEMRSSI);
			res = "" + String(v);
		}
	} catch (const std::exception& ex) {}
	if (checkNull && res.charAt(0) == char(255)) res = "";
	return res;
}





long Tools__Random(long a, long b) {
	long d;
	if (a > b) {
		d = a;
		a = b;
		b = d;
	}
	d = (b - a) + 1;
	try {
		return a + (esp_random() % d);
	} catch (const std::exception& ex) {
		if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Random Error: " + String(ex.what()));
	}
	return a + (rand() % d);
}

void Tools__Reboot() {
	// Dvc__LastStatusSave();
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("!!!!!!!! Rquested To REBOOT !!!!!!!!");
	delay(100);
	ESP.restart();
}



//-----------------------------------------------------------------------------------------------
//-------------------------------- Set/Get Settings From EPPROM  --------------------------------
//-----------------------------------------------------------------------------------------------

void Tools__SettingRead() {
	//...................................... Load Important Data
	_Configured = (MemoReadByte(EP_CONFIGURED) == 1 ? 1 : 0);

	_StartMode = Tools__Memory_StrGet("_StartMode");

	//...................................... Read Other Settings
	_Extender = (MemoReadByte(EP_EXTENDER) == 1 ? 1 : 0);
	_Circuit = Tools__Memory_StrGet("_Circuit");
	_ProjectID = Tools__Memory_StrGet("_ProjectID");
	_EncryptionKey = Tools__Memory_StrGet("_EncryptionKey");

	_ModemRSSI = MemoReadByte(EP_MODEMRSSI);
	if (_ModemRSSI <= 0 || _ModemRSSI > 99) _ModemRSSI = 99;
	_ModemSSID = Tools__Memory_StrGet("_ModemSSID");
	_ModemPass = Tools__Memory_StrGet("_ModemPass");
	_ModemChannel = Tools__StringToByte(Tools__Memory_StrGet("_ModemChannel"));
	if (_ModemChannel > 11 || _ModemChannel < 0) _ModemChannel = 0;

	_MeshRSSI = MemoReadByte(EP_MESHRSSI);
	if (_MeshRSSI <= 0 || _MeshRSSI > 99) _MeshRSSI = 99;
	_MeshChannel = Tools__StringToByte(Tools__Memory_StrGet("_MeshChannel"));
	if (_MeshChannel > 11 || _MeshChannel < 1) _MeshChannel = 1;
	_MeshChannelOld = Tools__StringToByte(Tools__Memory_StrGet("_MeshChannelOld"));
	if (_MeshChannelOld > 11 || _MeshChannelOld < 1) _MeshChannelOld = 1;
	_MeshName = Tools__Memory_StrGet("_MeshName");
	_MeshPass = Tools__Memory_StrGet("_MeshPass");
	if (_MeshPass == "") _MeshPass = DVC_DEFAULTPASS;
	_MeshNameOld = Tools__Memory_StrGet("_MeshNameOld");
	_MeshPassOld = Tools__Memory_StrGet("_MeshPassOld");

	_MqttBroker = Tools__Memory_StrGet("_MqttBroker");
	_MqttUser = Tools__Memory_StrGet("_MqttUser");
	_MqttPass = Tools__Memory_StrGet("_MqttPass");
}



void Tools__SettingDefault() {
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Setting To Dafault >>> ");
	//..................
	_Extender = 0;
	_Circuit = "wired";
	_ProjectID = "";
	_EncryptionKey = "";

	_ModemSSID = "";
	_ModemPass = "";
	_ModemChannel = 0;

	_MeshChannel = 1;
	_MeshChannelOld = 1;
	_MeshName = "";
	_MeshPass = DVC_DEFAULTPASS;

	_MqttBroker = "";
	_MqttUser = "";
	_MqttPass = "";
}



void Tools__SettingSave() {
	try {
		Tools__Memory_StrSet("_Extender", _Extender == 1 ? "1" : "0");
		Tools__Memory_StrSet("_Circuit", _Circuit);
		Tools__Memory_StrSet("_ProjectID", _ProjectID);
		Tools__Memory_StrSet("_EncryptionKey", _EncryptionKey);

		Tools__Memory_StrSet("_ModemChannel", String(_ModemChannel));
		Tools__Memory_StrSet("_ModemSSID", _ModemSSID);
		Tools__Memory_StrSet("_ModemPass", _ModemPass);

		Tools__Memory_StrSet("_MeshChannel", String(_MeshChannel));
		Tools__Memory_StrSet("_MeshName", _MeshName);
		Tools__Memory_StrSet("_MeshPass", _MeshPass);

		Tools__Memory_StrSet("_MqttBroker", _MqttBroker);
		Tools__Memory_StrSet("_MqttUser", _MqttUser);
		Tools__Memory_StrSet("_MqttPass", _MqttPass);
	} catch (const std::exception& ex) {
	}
}



void Tools__SettingFromJSON(String json) {
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("JSON ******************");
	Class_Json obj;
	const char* tmp;
	DynamicJsonDocument doc(1024 + json.length());
	doc.clear();
	deserializeJson(doc, json.c_str());
	tmp = doc["circuit"];
	_Circuit = String(tmp);
	tmp = doc["projectId"];
	_ProjectID = String(tmp);
	tmp = doc["encryptionKey"];
	_EncryptionKey = String(tmp);

	tmp = doc["modemSsid"];
	_ModemSSID = String(tmp);
	tmp = doc["modemPass"];
	_ModemPass = String(tmp);
	_ModemChannel = (byte)doc["modemChannel"];

	_MeshChannel = (byte)doc["meshChannel"];
	tmp = doc["meshtype"];
	tmp = doc["meshName"];
	_MeshName = String(tmp);
	tmp = doc["meshPass"];
	_MeshPass = String(tmp);

	tmp = doc["mqttBroker"];
	_MqttBroker = String(tmp);
	tmp = doc["mqttUser"];
	_MqttUser = String(tmp);
	tmp = doc["mqttPass"];
	_MqttPass = String(tmp);
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("********************* JSON OK");
}



void Tools__SettingShowInfo() {
	if (SOFTWARE_DEBUGMODE) {
		Debug_ConsolePrintln("\r\n_SerialNo: " + _SerialNo);
		Debug_ConsolePrintln("_StartMode: " + _StartMode);
		Debug_ConsolePrintln("Token: " + _Token);

		Debug_ConsolePrintln("\r\n_Circuit: " + _Circuit);
		Debug_ConsolePrintln("_Extender: " + String(_Extender));
		Debug_ConsolePrintln("_ProjectID: " + _ProjectID);
		Debug_ConsolePrintln("_EncryptionKey: " + _EncryptionKey);

		Debug_ConsolePrintln("\r\n_ModemChannel: " + String(_ModemChannel));
		Debug_ConsolePrintln("_ModemSSID: " + _ModemSSID);
		Debug_ConsolePrintln("_ModemPass: " + _ModemPass);
		Debug_ConsolePrintln("_ModemRSSI: " + String(_ModemRSSI));

		Debug_ConsolePrintln("\r\n_MeshChannel: " + String(_MeshChannel));
		Debug_ConsolePrintln("_MeshName: " + _MeshName);
		Debug_ConsolePrintln("_MeshPass: " + _MeshPass);
		Debug_ConsolePrintln("_MeshRSSI: " + String(_MeshRSSI));
		Debug_ConsolePrintln("_MeshNameOld: " + _MeshNameOld);
		Debug_ConsolePrintln("_MeshPassOld: " + _MeshPassOld);
		Debug_ConsolePrintln("_MeshChannelOld: " + String(_MeshChannelOld));

		Debug_ConsolePrintln("\r\n_MqttBroker: " + _MqttBroker);
		Debug_ConsolePrintln("_MqttUser: " + _MqttUser);
		Debug_ConsolePrintln("_MqttPass: " + _MqttPass);
		Debug_ConsolePrintln("");
	}
}



//===========================================================================================
//                                       GENERAL SETUP
//===========================================================================================
void Tools__TaskSetup() {
	Tools__Reboot_Task.disable();
	_TaskRunner.addTask(Tools__Reboot_Task);
	Tools__Reboot_Task.disable();

	//...................................
	//_TaskRunner.addTask(Tools__FreeMemory_Task);
	//Tools__FreeMemory_Task.enableIfNot();
}



String Tools__DataToString(char scope[], char type[], char data[], char sender[] = "") {
	CA_LowerCase(scope);
	String JsonText = "";
	DynamicJsonDocument doc(1024);
	doc["Scope"] = scope;
	doc["Type"] = type;
	doc["Data"] = data;
	doc["Sender"] = (strlen(sender) == 0 ? _MySenderID : sender);
	serializeJson(doc, JsonText);
	// String JsonText = "{\"Scope\":\"" + scope + "\",\"Type\":\"" + type + "\",\"Data\":\"" + data + "\",\"DataVersion\":1,\"Sender\":\"" + sender + "\"}";
	return JsonText;
}



void Tools__DataToJSON(Class_Json& obj, String str) {
	char Kscope[] = "\"Scope\"";  // KeyLen= 7 & Memory= 10 Bytes
#define JSONPACK_KEYLEN_SCOPE 7
#define JSONPACK_BYTES_SCOPE 10
	//..................
	char Ktype[] = "\"Type\"";  // KeyLen= 6 & Memory= 10 Bytes
#define JSONPACK_KEYLEN_TYPE 6
#define JSONPACK_BYTES_TYPE 10
	//..................
	char Kdata[] = "\"Data\"";  // KeyLen= 6 & Memory= 1024 Bytes
#define JSONPACK_KEYLEN_DATA 6
#define JSONPACK_BYTES_DATA 1024
	//..................
	char Ksender[] = "\"Sender\"";  // KeyLen= 8 & Memory= 23 Bytes
#define JSONPACK_KEYLEN_SENDER 8
#define JSONPACK_BYTES_SENDER 23
	//..................
	obj.Scope[0] = '\0';
	obj.Scope[JSONPACK_BYTES_SCOPE] = '\0';
	obj.Type[0] = '\0';
	obj.Type[JSONPACK_BYTES_TYPE] = '\0';
	obj.Data[0] = '\0';
	obj.Data[JSONPACK_BYTES_DATA] = '\0';
	obj.Sender[0] = '\0';
	obj.Sender[JSONPACK_BYTES_SENDER] = '\0';
	char c = str.charAt(0);
	int i, j, k, l, m, len = str.length() - 1;
	//........................................ Start & End Of JSON
	while (c != '{' && len > 0) {
		str = str.substring(1);
		len = str.length() - 1;
		c = str.charAt(0);
	}
	c = str.charAt(len);
	while (c != '}' && len > 0) {
		str = str.substring(0, len);
		len = str.length() - 1;
		c = str.charAt(len);
	}
	int max = len - 9;  // longest key = 8char(JSONPACK_KEYLEN_SENDER) & 1char(endSign'}')
	//........................................
	for (i = 1; i <= max; i++) {
		c = str.charAt(i);
		if (c != '\"') continue;
		//........................... Scope
		if (obj.Scope[0] == '\0') {
			l = JSONPACK_KEYLEN_SCOPE;
			m = JSONPACK_BYTES_SCOPE;
			for (j = 0; j < l; j++) {
				k = i + j;
				if (str.charAt(k) != Kscope[j]) break;
			}
			if (j == l) {
				c = str.charAt(++k);
				while (c == ' ' && k < max) c = str.charAt(++k);  // jump space
				if (c == ':') {
					c = str.charAt(++k);
					while (c == ' ' && k < max) c = str.charAt(++k);  // jump space
					for (++k, j = 0; k < len && j < m; j++, k++) {
						c = str.charAt(k);
						if (c == '\"') break;
						obj.Scope[j] = c;
					}
					obj.Scope[j] = '\0';
				}
				i = k;
				continue;
			}
		}
		//........................... Type
		if (obj.Type[0] == '\0') {
			l = JSONPACK_KEYLEN_TYPE;
			m = JSONPACK_BYTES_TYPE;
			for (j = 0; j < l; j++) {
				k = i + j;
				if (str.charAt(k) != Ktype[j]) break;
			}
			if (j == l) {
				c = str.charAt(++k);
				while (c == ' ' && k < max) c = str.charAt(++k);  // jump space
				if (c == ':') {
					c = str.charAt(++k);
					while (c == ' ' && k < max) c = str.charAt(++k);  // jump space
					for (++k, j = 0; k < len && j < m; j++, k++) {
						c = str.charAt(k);
						if (c == '\"') break;
						obj.Type[j] = c;
					}
					obj.Type[j] = '\0';
				}
				i = k;
				continue;
			}
		}
		//........................... Data
		if (obj.Data[0] == '\0') {
			l = JSONPACK_KEYLEN_DATA;
			m = JSONPACK_BYTES_DATA;
			for (j = 0; j < l; j++) {
				k = i + j;
				if (str.charAt(k) != Kdata[j]) break;
			}
			if (j == l) {
				c = str.charAt(++k);
				while (c == ' ' && k < max) c = str.charAt(++k);  // jump space
				if (c == ':') {
					c = str.charAt(++k);
					while (c == ' ' && k < max) c = str.charAt(++k);  // jump space
					for (++k, j = 0; k < len && j < m; j++, k++) {
						c = str.charAt(k);
						if (c == '\"') break;
						obj.Data[j] = c;
					}
					obj.Data[j] = '\0';
				}
				i = k;
				continue;
			}
		}
		//........................... Sender
		if (obj.Sender[0] == '\0') {
			l = JSONPACK_KEYLEN_SENDER;
			m = JSONPACK_BYTES_SENDER;
			for (j = 0; j < l; j++) {
				k = i + j;
				if (str.charAt(k) != Ksender[j]) break;
			}
			if (j == l) {
				c = str.charAt(++k);
				while (c == ' ' && k < max) c = str.charAt(++k);  // jump space
				if (c == ':') {
					c = str.charAt(++k);
					while (c == ' ' && k < max) c = str.charAt(++k);  // jump space
					for (++k, j = 0; k < len && j < m; j++, k++) {
						c = str.charAt(k);
						if (c == '\"') break;
						obj.Sender[j] = c;
					}
					obj.Sender[j] = '\0';
				}
				i = k;
				continue;
			}
		}
	}
}



/*
  Tools__DataToJSON__Old(Class_Json obj, String str) {
	int strLen = str.length();
	if (strLen > 2) {
		if (str.charAt(0) == '{')// && str.substring(strLen - 1, 1) == "}") {  // Is JSON
			DynamicJsonDocument doc(1024 + sizeof(str));
			doc.clear();
			deserializeJson(doc, str.c_str());
			strcpy(obj.Scope, doc["Scope"]);
			strcpy(obj.Type, doc["Type"]);
			strcpy(obj.Data, doc["Data"]);
			strcpy(obj.Sender, doc["Sender"]);
		}
	}
  }
*/

void Tools__ModbusSend(String dataHex, String circuit) {
	circuit.toUpperCase();
	//..................................................... Regular
	if (circuit == "" || circuit == "AUTO") {
		if (_Started_IotRun) {
			Mqtt__Send("dvcapp", "modbus", dataHex, "");
		}
		if (_Started_MeshRun) {
			Mesh__Send("modbus", dataHex);
		}
	}
	//..................................................... Extender
	else if (circuit == "MANUAL") {
		if (_Started_IotRun) {
			Mqtt__Send("appdvc", "modbus", dataHex, "");
			delay(200);
			Mqtt__Send("dvcapp", "modbus", dataHex, "");
		}
		if (_Started_MeshRun) {
			Mesh__Send("modbus", dataHex);
		}
	}
	//..................................................... Extender
	else {
		if (_Need_Extender) {
			if (_Started_IotRun && circuit == "IOT") {
				Mqtt__Send("dvcapp", "modbus", dataHex, "");
			} else if (_Started_MeshRun && circuit == "MESH") {
				Mesh__Send("modbus", dataHex);
			}
		}
	}
	// Tools__FreeMemory();
}




String Tools__ModbusCRC(String Data) {
	uint16_t Tools__ModbusCRC = 0xFFFF;
	int n = Data.length();  // lenght Data
	for (int i = 0; i < n; i = i + 2) {
		String p = Data.substring(i, i + 2);
		Tools__ModbusCRC ^= Tools__HexToUInt64(p);
		for (int i = 8; i != 0; i--) {               // Loop over each bit
			if ((Tools__ModbusCRC & 0x0001) != 0) {  // If the LSB is set
				Tools__ModbusCRC >>= 1;              // Shift right and XOR 0xA001
				Tools__ModbusCRC ^= 0xA001;
			} else                       // Else LSB is not set
				Tools__ModbusCRC >>= 1;  // Just shift right
		}
	}
	String CRC = String(Tools__ModbusCRC, HEX);
	CRC = "000" + CRC;
	CRC = CRC.substring(CRC.length() - 4, CRC.length());  //4 digit HEX
	CRC.toUpperCase();
	String CRCH = CRC.substring(2, 4);
	String CRCL = CRC.substring(0, 2);
	CRC = CRCH + CRCL;  //swap high & low bytes
	return CRC;
}



String Tools__ExtenderRank() {
	String data = "";                   // 28 char : (3b)FFFFFF (3b)ExM/ExI (2b)RSSI (10b)SN (7b)CPUID (3b)Random
	String header = "" + ((char)0xFF);  // FF
	header += header + header;          // FFFFFF
	try {
		String rssi = "99";
		if (_Need_Mesh) {
			header += "ExM";
			if (_MeshRSSI > 99 || _MeshRSSI <= 0) _MeshRSSI = 99;
			rssi = String(_MeshRSSI);
		} else {
			header += "ExI";
			if (_ModemRSSI > 99 || _ModemRSSI <= 0) _ModemRSSI = 99;
			rssi = String(_ModemRSSI);
		}
		//....................................... On Err Data (RSSI + SN + ChipId + Random)
		data = String("99") + String("9998887776") + String("9999999") + String("888");
		//....................................... Rssi
		int len = rssi.length();
		if (len != 2) {
			if (len > 2 || len == 0) rssi = "99";
			else rssi = "0" + rssi;
		}
		if (rssi == "00") rssi = "99";
		//....................................... chip
		String chip = _Token.substring(0, 7);
		if (chip == "0000000") chip = "9999999";
		//....................................... random
		chip += _Token.substring(7, 10);
		//....................................... OK Data
		data = rssi + _SerialNo + chip;
	} catch (const std::exception& ex) {
		if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("RankData Error: " + String(ex.what()));
	}
	return header + (data + "9999999999999999999999999").substring(0, 25);
}



void Tools__FreeMemory() {
	try {
		ESP.getFreeHeap();
		// heap_caps_free(NULL);
	} catch (const std::exception& ex) {}
}



void Tools__WifiPower(bool SetToOn) {
	if (SetToOn) {
		//................................. Sets WiFi Power-Sleep to None
		WiFi.setSleep(WIFI_PS_NONE);
		//................................. Sets WiFi RF power output to highest level
		WiFi.setTxPower(WIFI_POWER_19_5dBm);
		// WiFi.setTxPower(WIFI_POWER_2dBm);  // Sets WiFi RF power output to low level
	} else {
		WiFi.mode(WIFI_OFF);
	}
}



void Tools__SetMode(String mode, bool restartNeed) {
	if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Set Next Mode To : " + mode);
	//.............................................
	if (mode == "normal") {
	}
	//.............................................
	Tools__Memory_StrSet("_StartMode", mode);
	if (restartNeed) {
		Tools__Reboot_Task.enableIfNot();
	}
}



uint32_t Tools__GetChipID() {
	uint32_t chipId = 0;
	for (int i = 0; i < 17; i = i + 8) {
		chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}
	return chipId;
}


String Tools__ESPGetTemprature() {
	try {
		uint8_t f = temprature_sens_read();
		int c = (f - 32) * 0.555555;
		return (String(f) + " F / " + String(c) + " C");
	} catch (const std::exception& ex) {}
	return "? F / ? C";
}


String Tools__SerialBarcodeRead() {
	String barcode = "";
	for (byte i = 0; i < 10; i++) barcode += Tools__ByteToHexstring(MemoReadByte(i + EP_SERIAL_S));
	barcode.toUpperCase();
	return barcode;
}



void Tools__ScenarioLoad() {
	byte i, j, jj, n;
	int st;
	String tmp;
	for (i = 0; i < 10; i++) {
		Class_Scenario sen;
		sen.Id = (i < 7 ? i : i + 94);
		n = MemoReadByte(EP_SCENARIO_ACT_S + i);
		//..................................................... Act-Scenario Load
		sen.Act = (n == 0 || n == 1 ? n : 0xFF);
		if (sen.Act != 0xFF) {
			//................................................. Name-Scenario Load
			st = EP_SCENARIO_NAME_S + (12 * i);
			String tmp = MemoReadString(st, st + 11);
			tmp.trim();
			CA_CopyStr(sen.Name, tmp, 12);
			//................................................. Relay-Scenario Load
			for (j = 0; j < RELAY_MAX; j++) {
				st = EP_SCENARIO_RELAY_S + (RELAY_MAX * i) + j;
				n = MemoReadByte(st);
				sen.Relay[j] = n;
			}
			//................................................. Virtual-Scenario Load
			for (j = 0; j < 3; j++) {
				st = EP_SCENARIO_VIRTUAL_S + (3 * i) + j;
				n = MemoReadByte(st);
				sen.Virtual[j] = n;
			}
			//................................................. Code-Scenario Load
			for (j = 0; j < 3; j++) {
				st = EP_SCENARIO_CODESTATE_S + (3 * i) + j;
				n = (n == 1 || n == 2 ? n : 0xFF);
				n = MemoReadByte(st);
				sen.CodeCmd[j] = n;
				st = EP_SCENARIO_CODETEXT_S + (33 * i) + (j * 11);
				tmp = "";
				if (n != 0xFF) {
					n = MemoReadByte(st);
					if (n <= 10) {
						for (jj = 1; jj <= n; jj++) {
							tmp += Tools__ByteToHexstring(MemoReadByte(st + jj));
						}
					}
				}
				CA_CopyStr(sen.CodeVal[j], tmp);
			}
		}
		_Scenario_List[i] = sen;
	}
}


void Tools__ScenarioSave(byte s = 255) {
	byte i, j, jj, k, n, istart = 0, iend = 9;
	int st;
	String tmp;
	if (s != 255) {
		istart = s;
		iend = s;
	}
	//......................................................... Name-Scenario Save
	for (i = istart; i <= iend; i++) {
		Class_Scenario sen = _Scenario_List[i];
		MemoWriteByte(EP_SCENARIO_ACT_S + i, sen.Act);
		tmp = CA_ToString(sen.Name, 0, 12);
		tmp = tmp.substring(0, 12);
		st = EP_SCENARIO_NAME_S + (12 * i);
		MemoWriteString(st, st + 11, tmp);
		//..................................................... Relay-Scenario Save
		for (j = 0; j < RELAY_MAX; j++) {
			st = EP_SCENARIO_RELAY_S + (RELAY_MAX * i) + j;
			MemoWriteByte(st, sen.Relay[j]);
		}
		//..................................................... Virtual-Scenario Save
		for (j = 0; j < 3; j++) {
			st = EP_SCENARIO_VIRTUAL_S + (3 * i) + j;
			MemoWriteByte(st, sen.Virtual[j]);
		}
		//..................................................... Code-Scenario Save
		for (j = 0; j < 3; j++) {
			n = sen.CodeCmd[j];
			n = (n == 1 || n == 2 ? n : 0xFF);
			st = EP_SCENARIO_CODESTATE_S + (3 * i) + j;
			MemoWriteByte(st, n);
			st = EP_SCENARIO_CODETEXT_S + (33 * i) + (j * 11);
			if (n == 0xFF) {
				MemoWriteByte(st, 0xFF);
				MemoWriteString(st + 1, st + 10, "");
			} else {
				tmp = CA_ToString(sen.CodeVal[j]);
				tmp.trim();
				if (tmp.length() > 20) tmp = tmp.substring(0, 20);
				MemoWriteByte(st, tmp.length() / 2);
				for (jj = 1, k = 0; jj <= 10 && k < tmp.length(); k += 2, jj++) {
					MemoWriteByte(st + jj, Tools__HexToDecimal(tmp.substring(k, k + 2)) & 0xFF);
				}
			}
		}
	}
}


void Tools__DvcAddressLoad(String typ = "all") {
	typ.toLowerCase();
	byte i, j, num;
	String tmp = "";
	//......................................................... Virtual  Address-Load
	if (typ == "all" || typ == "virtual") {
		_Dvc_Virtual.Subnet = 0;
		_Dvc_Virtual.Address = 254;
		_Dvc_Virtual.Channel = 252;
	}
	//......................................................... Relay  Address-Load
	if (typ == "all" || typ == "relay") {
		for (i = 0; i < RELAY_MAX; i++) {
			_Dvc_Relay[i].Type = MemoReadByte(EP_RELAY_TYPE_S + i);
			_Dvc_Relay[i].Subnet = MemoReadByte(EP_RELAY_SUBNET_S + i);
			_Dvc_Relay[i].Address = MemoReadByte(EP_RELAY_ADDRESS_S + i);
			_Dvc_Relay[i].Channel = MemoReadByte(EP_RELAY_CHANNEL_S + i);
			_Dvc_Relay[i].Hold = (MemoReadByte(EP_RELAY_HOLDMIN_S + i) * 60) + MemoReadByte(EP_RELAY_HOLDSEC_S + i);
			if (_Dvc_Relay[i].Type == 0xFF && _Dvc_Relay[i].Subnet == 0xFF && _Dvc_Relay[i].Address == 0xFF) {
				_Dvc_Relay[i].Type = 0x00;
				_Dvc_Relay[i].Subnet = 0;
				_Dvc_Relay[i].Address = 1;
				_Dvc_Relay[i].Channel = i + RELAY_CH_START;
			}
			num = MemoReadByte(EP_RELAY_LOCK_S + i);
			_Dvc_Relay[i].Lock = (num == 1 ? true : false);
		}
	}
}


void Tools__DvcAddressSave(String typ = "all") {
	typ.toLowerCase();
	byte i, num;
	String tmp = "";
	//......................................................... Relay  Address-Save
	if (typ == "all" || typ == "relay") {
		for (i = 0; i < RELAY_MAX; i++) {
			MemoWriteByte(EP_RELAY_TYPE_S + i, _Dvc_Relay[i].Type);
			MemoWriteByte(EP_RELAY_SUBNET_S + i, _Dvc_Relay[i].Subnet);
			MemoWriteByte(EP_RELAY_ADDRESS_S + i, _Dvc_Relay[i].Address);
			MemoWriteByte(EP_RELAY_CHANNEL_S + i, _Dvc_Relay[i].Channel);
			MemoWriteByte(EP_RELAY_HOLDSEC_S + i, _Dvc_Relay[i].Hold % 60);
			MemoWriteByte(EP_RELAY_HOLDMIN_S + i, _Dvc_Relay[i].Hold / 60);
			MemoWriteByte(EP_RELAY_LOCK_S + i, _Dvc_Relay[i].Lock);
		}
	}
}


void Tools__RunsenLoad(String typ = "all") {
	typ.toLowerCase();
	byte b, i, j, k, num;
	int st;
	String tmp = "";
	//......................................................... Virtual  Runsen-Load
	if (typ == "all" || typ == "virtual") {
		for (i = 0; i < 10; i++) {
			num = MemoReadByte(EP_RUNSENVIRTUAL_CON_S + i);
			if (num == 0xFF || num == 0) {
				_Runsen_Virtual[i].Dvc = 0xFF;
				_Runsen_Virtual[i].Con = 0xFF;
				_Runsen_Virtual[i].Sen = 0xFF;
			} else {
				_Runsen_Virtual[i].Dvc = 0x01;
				_Runsen_Virtual[i].Con = num;
				_Runsen_Virtual[i].Sen = MemoReadByte(EP_RUNSENVIRTUAL_SEN_S + i);
			}
		}
	}
	//......................................................... Code  Runsen-Load
	if (typ == "all" || typ == "code") {
		for (i = 0; i < 5; i++) {
			num = MemoReadByte(EP_RUNSENCODE_TYPE_S + i);
			num = (num == 1 || num == 2 ? num : 0xFF);
			_Runsen_Code[i].Dvc = num;
			st = EP_RUNSENCODE_CON_S + (11 * i);
			tmp = "";
			if (num == 0xFF) {
				_Runsen_Code[i].Sen = 0xFF;
			} else {
				num = MemoReadByte(st);
				if (num > 10) {
					_Runsen_Code[i].Sen = 0xFF;
				} else {
					_Runsen_Code[i].Sen = MemoReadByte(EP_RUNSENCODE_SEN_S + i);
					for (j = 1; j <= num; j++) {
						tmp += Tools__ByteToHexstring(MemoReadByte(st + j));
					}
				}
			}
			CA_CopyStr(_Runsen_Code[i].Con, tmp);
		}
	}
}


void Tools__RunsenSave(String typ = "all") {
	typ.toLowerCase();
	byte i, j, k, num;
	int st;
	String tmp = "";
	//......................................................... Virtual  Runsen-Save
	if (typ == "all" || typ == "virtual") {
		for (i = 0; i < 10; i++) {
			MemoWriteByte(EP_RUNSENVIRTUAL_CON_S + i, _Runsen_Virtual[i].Con);
			MemoWriteByte(EP_RUNSENVIRTUAL_SEN_S + i, _Runsen_Virtual[i].Sen);
		}
	}
	//......................................................... Code  Runsen-Save
	if (typ == "all" || typ == "code") {
		for (i = 0; i < 5; i++) {
			num = _Runsen_Code[i].Dvc;
			num = (num == 1 || num == 2 ? num : 0xFF);
			MemoWriteByte(EP_RUNSENCODE_TYPE_S + i, num);
			st = EP_RUNSENCODE_CON_S + (11 * i);
			if (_Runsen_Code[i].Dvc == 0xFF) {
				MemoWriteByte(st, 0xFF);
				MemoWriteString(st + 1, st + 11, "");
				MemoWriteByte(EP_RUNSENCODE_SEN_S + i, 0xFF);
			} else {
				tmp = CA_ToString(_Runsen_Code[i].Con);
				tmp.trim();
				if (tmp.length() > 20) tmp = tmp.substring(0, 20);
				MemoWriteByte(st, tmp.length() / 2);
				for (j = 1, k = 0; j <= 10 && k < tmp.length(); k += 2, j++) {
					MemoWriteByte(st + j, Tools__HexToDecimal(tmp.substring(k, k + 2)) & 0xFF);
				}
				MemoWriteByte(EP_RUNSENCODE_SEN_S + i, _Runsen_Code[i].Sen);
			}
		}
	}
}
