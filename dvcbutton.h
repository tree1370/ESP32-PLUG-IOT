

//-----------------------------------------------------------------------------------------------
//--------------------------- Enable Set Address _Dvc_Line For Learn Each Key ------------------------
//-----------------------------------------------------------------------------------------------
void Manual__EnableSetAddressLine() {
  byte T = 0;
  _Dvc_IsEnableSetAddress = true;  // فعال کردن حالت لرن ابزار
  while (digitalRead(KeyIN[0]) == HIGH) {
    delay(100);
    T++;
    if (T > 30)  Manual_SwichToConfig();
  }
  T = 0;
  while (T++ < 100 && _Dvc_IsEnableSetAddress) {

    delay(100);
  }
  _Dvc_IsEnableSetAddress = false;  // فعال کردن حالت لرن ابزار

  Button__LEDShow();
  T = 0;
}


//-----------------------------------------------------------------------------------------------
//--------------------------- Learn EACH KEY ----------------------------
//-----------------------------------------------------------------------------------------------
void Manual__SetAddressLine(String sentence) {
  if (sentence != "") {
    byte Type = Tools__HexToDecimal(sentence.substring(8, 10));
    byte Fn = Tools__HexToDecimal(sentence.substring(2, 4));
    if (Fn == 240 && (Type == T_Light || Type == T_Siren || Type == T_RemoteRelay)) {  //for example 02F0000503
      byte add = Tools__HexToDecimal(sentence.substring(0, 2));
      byte sub = Tools__HexToDecimal(sentence.substring(4, 6));
      byte CH = Tools__HexToDecimal(sentence.substring(6, 8));
      byte Mode = Tools__HexToDecimal(sentence.substring(10, 12));
      byte Val = Tools__HexToDecimal(sentence.substring(12, 14));

      if (Type == T_Light || Type == T_Siren || Type == T_RemoteRelay) {
        MemoWriteByte(EP_RELAY_ADDRESS_S + _Dvc_Line, add);  //Save Address To EEPROM
        MemoWriteByte(EP_RELAY_SUBNET_S + _Dvc_Line, sub);   //Save Subnnet To EEPROM
        MemoWriteByte(EP_RELAY_CHANNEL_S + _Dvc_Line, CH);   //Save Channel To EEPROM
        MemoWriteByte(EP_RELAY_TYPE_S + _Dvc_Line, Type);    //Save Type To EEPROM
        _Dvc_Relay[_Dvc_Line].Address = add;
        _Dvc_Relay[_Dvc_Line].Channel = CH;
        _Dvc_Relay[_Dvc_Line].Subnet = sub;
        _Dvc_Relay[_Dvc_Line].Type = Type;
      }
      Button__LEDShow();

      _Dvc_IsEnableSetAddress = false;
    }
  }
}



//-----------------------------------------------------------------------------------------------
//--------------------------- Dvc__LOOP_LED ----------------------------
//-----------------------------------------------------------------------------------------------
void Dvc__LOOP_LED() {
  _Dvc_Led[0] = !_Dvc_Led[0];
  Button__LEDShow();
  delay(100);
}


//-----------------------------------------------------------------------------------------------
//--------------------------- Manual_SwichToConfig ----------------------------
//-----------------------------------------------------------------------------------------------
void Manual_SwichToConfig() {
  for (byte z = 0; z < 9; z++) {
    Dvc__LOOP_LED();
  }
  _Dvc_Led[0] = 0;
  Dvc__LOOP_LED();
  Tools__SetMode(_StartMode == "config_panel" ? "normal" : "config_panel", true);
}


//-----------------------------------------------------------------------------------------------
//--------------------------- Manual_SwichToConfig ----------------------------
//-----------------------------------------------------------------------------------------------
byte CounterConfig = 0;
void Manual__Loop(void* param) {

  if (SOFTWARE_DEBUGMODE) Serial.println("Manual Loop Start");
  while (true) {
    try {
      byte Count = 0;

      for (;;) {
        if (_Started_ConfigPanel && CounterConfig > 100) {
          Dvc__LOOP_LED();
          _Dvc_ConfigModeLED++;
          if (_Dvc_ConfigModeLED > 8) _Dvc_ConfigModeLED = 0;
          CounterConfig = 0;
        } else if (Manual_SwichToConfig && CounterConfig <= 100) {
          CounterConfig++;
        }

        for (byte i = 0; i < RELAY_MAX; i++) {
          if (digitalRead(KeyIN[i]) == HIGH) {
            if (SOFTWARE_DEBUGMODE) Serial.println("Key Press");
            byte Counter = 0;
            byte CH = i;
            delay(100);
            while (digitalRead(KeyIN[i]) == HIGH) {
              delay(100);
              Counter++;
              if (Counter > 30) {
                pixels.setPixelColor(CH, pixels.Color(20, 0, 0));
                pixels.show();
                _Dvc_Line = CH;
                Manual__EnableSetAddressLine();
              }
            }
            if (Counter < 30) {
              _Dvc_Led[CH] = !_Dvc_Led[CH];
              Dvc__RelayControl(CH);
              Dvc__SendData(1, CH, _Dvc_Relay[CH].Type);
              //MemoWriteByte(EP_LASTDVCMODE_S + CH, _Dvc_Led[CH]);
            }
          }
        }
        // digitalWrite(KeyOUT[i], HIGH);
      }

      if (Count >= 10) {  //Relay Remote Time
        Count = 0;
        for (byte i = 0; i < RELAY_MAX; i++) {
          if (_Dvc_RelayTimeout[i] < 1 && _Dvc_Relay[i].Type == T_RemoteRelay) {
            W_RelayOFF[i] = 1;
            Button__LEDShow();
            delay(30);
            W_RelayOFF[i] = 0;
            Button__LEDShow();
          }
          if (_Dvc_RelayTimeout[i] > 0)
            _Dvc_RelayTimeout[i]--;
        }
      }
      delay(100);
      Count++;
    } catch (const std::exception& ex) {}
  }
}


//_____________________________________ Check Virtual Code Input Senario _______________________
void Check_VirtualCode(byte Status) {
  for (byte i = 0; i < 10; i++) {
    if (_Runsen_Virtual[i].Dvc == 255) break;
    if (_Runsen_Virtual[i].Con == Status)
      Dvc__Senario_Runing(_Runsen_Virtual[i].Sen);
  }
}

//_____________________________________ Check Custom Code Input Senario _______________________
void Check_CustomCode(String Code) {
  for (byte i = 0; i < 5; i++) {
    if (_Runsen_Code[i].Dvc == 255) break;
    //    if (CA_Equal(_Runsen_Code[i].Con, Code))
    if (CA_ToString(_Runsen_Code[i].Con, 0, 20) == Code)
      Dvc__Senario_Runing(_Runsen_Code[i].Sen);
  }
}


//_____________________________________ Run Senario ______________________________________
void Dvc__Senario_Runing(byte Sen) {
  Sen--;
  if (_Scenario_List[Sen].Act != 1) return;

  byte z = 0;
  if (Sen == 8)  //Active Security Sen
    _Scenario_List[7].Act = 1;
  else if (Sen == 9)  //Deactive Security Sen
    _Scenario_List[7].Act = 0;

  for (z = 0; z < RELAY_MAX; z++) {  //Set Relay
    if (_Scenario_List[Sen].Relay[z] != 255) {
      switch (_Dvc_Relay[z].Type) {
        case 0:
          if (_Scenario_List[Sen].Relay[z] == 0) {
            _Dvc_Led[z] = 0;
            Dvc__RelayControl(z);
            Dvc__SendData(1, z, _Dvc_Relay[z].Type);
            MemoWriteByte(EP_LASTDVCMODE_S + z, _Dvc_Led[z]);
          } else if (_Scenario_List[Sen].Relay[z] == 1) {
            _Dvc_Led[z] = 1;
            Dvc__RelayControl(z);
            Dvc__SendData(1, z, _Dvc_Relay[z].Type);
            MemoWriteByte(EP_LASTDVCMODE_S + z, _Dvc_Led[z]);
          } else if (_Scenario_List[Sen].Relay[z] == 2) {
            _Dvc_Led[z] = !_Dvc_Led[z];
            Dvc__RelayControl(z);
            Dvc__SendData(1, z, _Dvc_Relay[z].Type);
            MemoWriteByte(EP_LASTDVCMODE_S + z, _Dvc_Led[z]);
          }
          break;
      }
      delay(100);
    }
  }

  for (z = 0; z < 3; z++) {  //Send RS485 Zooril Virtual Code
    if (_Scenario_List[Sen].Virtual[z] == 255) continue;
    String Send_Data = "FEF200FC110200" + Tools__ByteToHexstring(_Scenario_List[Sen].Virtual[z]);
    Send_Data += Tools__ModbusCRC(Send_Data);
    Tools__ModbusSend(Send_Data, "Auto");
    delay(100);
  }

  for (z = 0; z < 3; z++) {  //Send RS485 Custom Code
    if (_Scenario_List[Sen].CodeCmd[z] == 255) continue;

    String Send_Data = CA_ToString(_Scenario_List[Sen].CodeVal[z]);
    Tools__ModbusSend(Send_Data, "Auto");
    delay(100);
  }
}


//===========================================================================================
//                                          Manual Mode
//===========================================================================================
void Manual__Setup() {
  //  delay(1000);
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();  // Set all pixel colors To Off
  pixels.show();

  Button__LEDShow();  //Set LED On or Off
  //  _Dvc_Led[0] = 2;
  pixels.setPixelColor(0, pixels.Color(0, 20, 20));
  pixels.show();

  for (byte i = 0; i < RELAY_MAX; i++) {
    pinMode(KeyIN[i], INPUT_PULLUP);
  }

  pixels.setPixelColor(0, pixels.Color(0, 20, 20));
  pixels.show();


  for (byte i = 0; i < RELAY_MAX; i++) {  //Read Subnnet & Address & Channel & LockMode as EEPROM
    try {
      if (MemoReadByte(i + EP_RELAY_MODE_S) == 1) _Dvc_Led[i] = 0;
      else if (MemoReadByte(i + EP_RELAY_MODE_S) == 2) _Dvc_Led[i] = 1;
      else if (MemoReadByte(i + EP_RELAY_MODE_S) == 0) _Dvc_Led[i] = MemoReadByte(i + EP_LASTDVCMODE_S);
      else MemoWriteByte(i + EP_RELAY_MODE_S, 0);

      if (_Dvc_Relay[i].Type != T_Light && _Dvc_Relay[i].Type != T_Siren) _Dvc_Led[i] = 0;
      if (_Dvc_Relay[i].Type != T_RemoteRelay) Dvc__RelayControl(i);

      Dvc__SendData(1, i, _Dvc_Relay[i].Type);
    } catch (...) {}
  }

  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.show();

  xTaskCreatePinnedToCore(
    Manual__Loop,    // Function that should be called
    "Manual__Loop",  // Name of the task (for debugging)
    2048,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    NULL,            // Task handle
    1);
  if (SOFTWARE_DEBUGMODE) Serial.println("Manual Setup End");
}


//-----------------------------------------------------------------------------------------------
//--------------------------- Button__LEDShow ----------------------------
//-----------------------------------------------------------------------------------------------
void Button__LEDShow() {

  for (byte i = 0; i < RELAY_MAX; i++) {
    if (_Dvc_Led[i] == 0) pixels.setPixelColor(i, pixels.Color(0, 20, 0));
    else pixels.setPixelColor(i, pixels.Color(10, 10, 10));
    pixels.show();
  }

}

////****************************************************************************************************************************************************************
