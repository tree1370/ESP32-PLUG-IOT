
#define SOFTWARE_DEBUGMODE true
#define SOFTWARE_VERSION "1.51F R10"
//===========================================================================================
//                                           EPPROM
//===========================================================================================
#define EEPROM_SIZE 1200

#define EP_CONFIGURED 100
#define EP_STARTMODE 101

#define EP_EXTENDER 102
#define EP_CIRCUIT 103
#define EP_SERIAL_S 111
#define EP_SERIAL_E 120
#define EP_PROJECTID_S 121
#define EP_PROJECTID_E 140
#define EP_ENCRYPTIONKEY_S 141
#define EP_ENCRYPTIONKEY_E 150

#define EP_MEMORYSTART 160
#define EP_MESHRSSI 160
#define EP_MESHCHANNEL 161
#define EP_MESHCHANNEL_OLD 162
// #define EP_MESHTYPE 163
#define EP_MESHNAME_S 171
#define EP_MESHNAME_E 190
#define EP_MESHNAME_OLD_S 191
#define EP_MESHNAME_OLD_E 210
#define EP_MESHPASS_S 211
#define EP_MESHPASS_E 230
#define EP_MESHPASS_OLD_S 231
#define EP_MESHPASS_OLD_E 250

#define EP_MQTTUSER_S 251
#define EP_MQTTUSER_E 270
#define EP_MQTTPASS_S 271
#define EP_MQTTPASS_E 290
#define EP_MQTTBROKER_S 301
#define EP_MQTTBROKER_E 400

#define EP_MODEMCHANNEL 401
#define EP_MODEMSSID_S 402
#define EP_MODEMSSID_E 434
#define EP_MODEMPASS_S 441
#define EP_MODEMPASS_E 460
#define EP_MODEMRSSI 461

#define EP_RELAY_SUBNET_S 521
#define EP_RELAY_SUBNET_E 530

#define EP_RELAY_ADDRESS_S 531
#define EP_RELAY_ADDRESS_E 540

#define EP_RELAY_CHANNEL_S 541
#define EP_RELAY_CHANNEL_E 550

#define EP_RELAY_TYPE_S 551
#define EP_RELAY_TYPE_E 560

#define EP_RELAY_HOLDSEC_S 561
#define EP_RELAY_HOLDSEC_E 570
#define EP_RELAY_HOLDMIN_S 571
#define EP_RELAY_HOLDMIN_E 580

#define EP_RELAY_LOCK_S 581
#define EP_RELAY_LOCK_E 590

#define EP_RELAY_MODE_S 591
#define EP_RELAY_MODE_E 600

#define EP_LASTDVCMODE_S 601
#define EP_LASTDVCMODE_E 610

#define EP_SCENARIO_NAME_S 611
#define EP_SCENARIO_NAME_E 630
#define EP_SCENARIO_ACT_S 631
#define EP_SCENARIO_ACT_E 640
#define EP_SCENARIO_RELAY_S 641
#define EP_SCENARIO_RELAY_E 740
#define EP_SCENARIO_VIRTUAL_S 741
#define EP_SCENARIO_VIRTUAL_E 770
#define EP_SCENARIO_CODESTATE_S 771
#define EP_SCENARIO_CODESTATE_E 775
#define EP_SCENARIO_CODETEXT_S 781
#define EP_SCENARIO_CODETEXT_E 1085

#define EP_RUNSENVIRTUAL_CON_S 1091
#define EP_RUNSENVIRTUAL_CON_E 1100
#define EP_RUNSENVIRTUAL_SEN_S 1101
#define EP_RUNSENVIRTUAL_SEN_E 1110

#define EP_RUNSENCODE_TYPE_S 1111
#define EP_RUNSENCODE_TYPE_E 1115
#define EP_RUNSENCODE_CON_S 1116
#define EP_RUNSENCODE_CON_E 1170
#define EP_RUNSENCODE_SEN_S 1171
#define EP_RUNSENCODE_SEN_E 1175


//===========================================================================================
//                                           CONSTANTS
//===========================================================================================
#define RELAY_MAX 1
#define RELAY_CH_START 81

#define MODEM_RSSI_ACCEPTABLE 95
#define DVC_CIRCUIT_AUTO_0 0
#define DVC_CIRCUIT_WIRED_1 1
#define DVC_CIRCUIT_WIFI_2 2
#define DVC_CIRCUIT_WIREFI_3 3
#define DVC_CIRCUIT_IOT_4 4

#define DVC_DEFAULTNAME "zooril"
#define DVC_DEFAULTPASS "00000000"

#define MESH__PORT 5555
#define MESH__HIDDEN 1
#define MESH__NOHIDE 0
#define MESH__MAXCON 5
#define MESH__MISSTIMEOUT 50  //  6 * 30sec = 3Min
#define NODE__MISSTIMEOUT 52  // 10 * 30sec = 5Min
#define HOSTNAME "MQTT_Bridge"

#define TASK_PRIORITY_HIGH 2        // 5
#define TASK_PRIORITY_MID 2         // 3
#define TASK_PRIORITY_LOW 2         // 2
#define TASK_STACKSIZE_SMALL 4096   // 1024
#define TASK_STACKSIZE_MEDIUM 8192  // 4096
#define TASK_STACKSIZE_LARGE 8192   // 8192
#define TASK_CORE_0 0
#define TASK_CORE_1 1
#define TASK_LOOP_DELAY 20

//===========================================================================================
//                                        GENERAL Tools
//===========================================================================================
byte _Fail = 0;
byte _TestRebootCount = 0;

void Debug_ConsolePrint(String p1);
void Debug_ConsolePrintln(String p1);

int Tools__StringSplitSize(String inp, char splitter);
void Tools__StringSplit(String inp, char splitter, String outArray[], int sizeOfArray);
byte Tools__StringToByte(String inp);
byte Tools__StringToCharVal(String inp, byte index);
uint64_t Tools__HexToUInt64(String p);
long Tools__HexToDecimal(String value);
String Tools__ByteToHexstring(long b);

void MemoClean(int p_start, int p_end);
void MemoWriteByte(int p_start, byte val);
void MemoWriteString(int p_start, int p_end, String val);
int MemoReadByte(int p_start);
String MemoReadString(int p_start, int p_end);
bool Tools__Memory_StrSet(String key, String val);
String Tools__Memory_StrGet(String key);
void Tools__SetMode(String mode, bool restartNeed);
void Tools__SettingRead();
void Tools__SettingDefault();
void Tools__SettingSave(String key);
void Tools__SettingFromJSON(String json);
String Tools__SerialBarcodeRead();


void Tools__ScenarioLoad();
void Tools__ScenarioSave(byte s);
void Tools__RnnsenLoad(String typ);
void Tools__RunsenSave(String typ);
void Tools__DvcAddressLoad(String typ);
void Tools__DvcAddressSave(String typ);



Scheduler _TaskRunner;
void TaskTimerLoop(void* param);
void Tools__Reboot();
Task Tools__Reboot_Task(TASK_SECOND * 2, TASK_FOREVER, &Tools__Reboot);
void Tools__TaskSetup();
void Tools__FreeMemory();
Task Tools__FreeMemory_Task(TASK_MINUTE * 3, TASK_FOREVER, &Tools__FreeMemory);
void Tools__WifiPower(bool SetToOn);
uint32_t Tools__GetChipID();
long Tools__Random(long a, long b);





class Class_Json {
  public:
	char Scope[11];
	char Type[11];
	char Data[1025];
	// int DataVersion = 1;
	char Sender[24];
};
Class_Json _DataRcv_JSON;
void Tools__DataToJSON(Class_Json& obj, String str);
String Tools__DataToString(char scope[], char type[], char data[], char sender[]);
String Tools__ModbusCRC(String Data);
void Tools__ModbusSend(String dataHex, String circuit);


class Class_Scenario {
  public:
	char Name[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	byte Id = 0;
	byte Act = 0xFF;
	#if RELAY_MAX==1
		byte Relay[1] = { 0xFF } ;
	#elif RELAY_MAX==2
		byte Relay[2] = { 0xFF, 0xFF };
	#elif RELAY_MAX==3
		byte Relay[3] = { 0xFF, 0xFF, 0xFF };
	#elif RELAY_MAX==4
		byte Relay[4] = { 0xFF, 0xFF,0xFF, 0xFF };
	#elif RELAY_MAX==5
		byte Relay[5] = { 0xFF, 0xFF,0xFF, 0xFF, 0xFF };
	#else
		byte Relay[6] = { 0xFF, 0xFF,0xFF, 0xFF, 0xFF, 0xFF };
	#endif
	byte Virtual[3] = { 0xFF, 0xFF, 0xFF };
	byte CodeCmd[3] = { 0xFF, 0xFF, 0xFF };
	char CodeVal[3][21] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
};
Class_Scenario _Scenario_List[10];

class Class_RunsenCode {
  public:
	byte Dvc = 0xFF;
	char Con[21] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	byte Sen = 0xFF;
};
class Class_Runsen {
  public:
	byte Dvc = 0xFF;  //if Dvc==255 ---> Not USE   Number(1-20) ---> Index Of DI
	byte Con = 0xFF;  //Value Condition
	byte Sen = 0xFF;  //Nubmber Of Senario
};
Class_Runsen _Runsen_Virtual[10];
Class_RunsenCode _Runsen_Code[5];

class Class_DvcBase {
  public:
	byte Type = 0xFF;
	byte Subnet = 0xFF;
	byte Address = 0xFF;
	byte Channel = 0xFF;
};
class Class_DvcRelay : public Class_DvcBase {
  public:
	int Hold = 0;
	bool Lock = 0;
};
Class_DvcRelay _Dvc_Relay[1];
Class_DvcBase _Dvc_Virtual;




//===========================================================================================
//                                        Config
//===========================================================================================
char _MySenderID[23];
String _Token = "";
String _StartMode = "";
String _SerialNo = "";

int _ConfigTimeout = 0;

byte _Configured = 0;
byte _Extender = 0;
String _Circuit = "";
String _ProjectID = "";
String _EncryptionKey = "";

String _ModemSSID = "";
String _ModemPass = "";
byte _ModemRSSI = 99;
byte _ModemChannel = 0;

String _MqttBroker = "";
String _MqttUser = "";
String _MqttPass = "";

byte _MeshRSSI = 99;
byte _MeshChannel = 1;
byte _MeshChannelOld = 1;
String _MeshName = DVC_DEFAULTNAME;
String _MeshPass = DVC_DEFAULTPASS;
String _MeshNameOld = DVC_DEFAULTNAME;
String _MeshPassOld = DVC_DEFAULTPASS;

//.......................
bool _Need_IOT = false;
bool _Need_Mesh = false;
bool _Need_Wire = false;
bool _Need_Extender = false;
bool _Need_Taskloop = false;

bool _Started_WiredOnly = false;
bool _Started_IotRun = false;
bool _Started_MeshRun = false;
bool _Started_ConfigPanel = false;
bool _Started_ConfigSendall = false;
bool _Started_ConfigRanking = false;
bool _Started_RebootSendall = false;

SimpleList<uint32_t> _Mesh_NodesList;


//===========================================================================================
//                                           Mesh
//===========================================================================================
painlessMesh _MeshObj;
String _Mesh_SendData;  // Public WifiMesh Data
String _Mesh_SendType;  // Public WifiMesh Type
bool _Mesh_IsConnected = false;
void Mesh__Setup(void* param);
void Mesh__PrepareWithTasks(String mname, String mpass, byte channel, bool isroot);
void Mesh__Connect();
Task Mesh__Connect_Task(TASK_SECOND * 5, TASK_FOREVER, &Mesh__Connect);
void Mesh__StopToReconnect();
void Mesh__OnReceive(const uint32_t& from, const String& msg);  // call back from mesh node in mesh network
void Mesh__Send(String type, String data);
void Mesh__SendByTask();
Task Mesh__SendTask(TASK_SECOND * 2, TASK_FOREVER, &Mesh__SendByTask);
void Mesh__CheckRootAlive();



//===========================================================================================
//                                         Mqtt / Root
//===========================================================================================
WiFiClient wifiClient;
PubSubClient _MqttObj(wifiClient);
String _CurrentIP = "0.0.0.0";
String _MqttTopic = "";
String _MqttClientId = "";
String _Mqtt_RootAliveRank = "";
String _Mqtt_RootAliveData = "";
byte _Mqtt_RootAliveCount = 0;
bool _Mqtt_RootReady = false;
byte _MqttCon_Steps = 0;
bool _MqttCon_IsConnected = false;
bool _Mqtt_TryConnecting = false;
uint16_t _MqttPort = 1883;
void Mqtt__OnRecieve(char* topic, byte* payload, unsigned int length);  // call back from mqtt broker
void Mqtt__Stop();
void Mqtt__Connect();
void Mqtt__Send(String scope, String type, String data, String sender);




//===========================================================================================
//                                         Mqtt / IOT
//===========================================================================================
void IOT__Setup(void* param);



//===========================================================================================
//                                     SendAll (On Old Mesh)
//===========================================================================================
void Sendall__Setup(void* param);
void Sendall__Loop();
void Sendall__ShareData();
void Sendall__ShareApRank();
void Sendall__ShareModemRank();
Task Sendall__ShareData_Task(TASK_SECOND * 15, TASK_FOREVER, &Sendall__ShareData);



//===========================================================================================
//                                       Config / Update
//===========================================================================================
WebServer _Server(80);
void Config__Setup(void* param);
void Config__Loop();
void Config__APIAlive();
void Config__APIGet();
void Config__APISet();
void Config__APIFirmware();
void Config__FileJquery();
void Config__FileMainScript();
void Config__FileSettingScript();
void Config__FileMainCSS();
void Config__PageHtml();
void Config__PageFirmware();




//===========================================================================================
//                                            Dvc
//===========================================================================================
#define T_Light 0
#define T_Siren 8
#define T_RemoteRelay 9

extern const String _Dvc_TypeCode;
extern const String DVC_TYPE;
extern const String DVC_DESC;
extern byte _Dvc_Phase;
byte _Dvc_Line = 0;
bool _Dvc_HasBarcode = false;
bool _Dvc_IsEnableSetAddress = false;
bool _Dvc_Led[1] = { 0 };  // LED now Mode(On or Off) ??????????????????
byte _Dvc_ConfigModeLED = 0;
byte _Dvc_ConfigCounter = 0;

int _Dvc_RelayTimeout[1] = { 0 };

#define PIN 22        // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 1  // Popular NeoPixel ring size
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

bool W_RelayON[1] = { 33 };
bool W_RelayOFF[1] = { 25 };
const byte KeyIN[1] = { 23 };


void Dvc__AnalyzeData(String Data, byte CircuitType);
void Dvc__SendData(byte Type_Circuit, byte LineNumber, byte TypeCode);
void Dvc__RelayControl(byte L);

void Dvc__Senario_Runing(byte Sen);
void Check_CustomCode(String Code);
void Check_VirtualCode(byte Status);

void Check_VirtualCode(byte Status);
void Check_CustomCode(String Code);
void Dvc__Senario_Runing(byte Sen);

//===========================================================================================
//                                          TouchKey
//===========================================================================================
int _Dvc_SetAddressCounter = 0;
void Manual__EnableSetAddressLine();
void Manual__SetAddressLine(String sentence);
void Button__LEDShow();
void Manual__Setup();
void Manual__Loop(void* param);
void Manual_SwichToConfig();
