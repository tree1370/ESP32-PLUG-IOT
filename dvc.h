
//===========================================================================================
//                                             FIX VARIBLE
//===========================================================================================
const String DVC_DESC = "Plug " + String(RELAY_MAX) + "Ch Sen";
const String DVC_TYPE = "Relay" + String(RELAY_MAX) + "Ch";
const String _Dvc_TypeCode = "313139";
byte _Dvc_Phase = 0;


//===========================================================================================
//                                          DVC Controls
//===========================================================================================

void Dvc__AnalyzeData(String Data, byte CircuitType) {
  if (Data == "") return;

  if (_Dvc_IsEnableSetAddress) {
    Manual__SetAddressLine(Data);
    return;
  }

  while (Data != "") {

    byte AA = Tools__HexToDecimal(Data.substring(0, 2));
    byte Fn = Tools__HexToDecimal(Data.substring(2, 4));
    byte Sub = Tools__HexToDecimal(Data.substring(4, 6));
    byte CH = Tools__HexToDecimal(Data.substring(6, 8));
    byte IO = Tools__HexToDecimal(Data.substring(8, 10));  //Request=0 & Response=17

    String Data2 = "";

    if (Fn == 242) {  //AA+Fn+Sub+CH+IO+Len+Val(Len)+CRC(2b)
      byte Len = Tools__HexToDecimal(Data.substring(10, 12));
      Data2 = Data.substring(0, 12 + (Len * 2) + 4);
      if (Data.length() > Data2.length())
        Data = Data.substring(Data2.length(), Data.length());
      else
        Data = "";
    } else if (Fn == 240) {  //AA+Fn+Sub+CH++Type+IO+Len+Val(Len)+CRC(2b)
      byte Len = Tools__HexToDecimal(Data.substring(12, 14));
      Data2 = Data.substring(0, 14 + (Len * 2) + 4);
      if (Data.length() > Data2.length())
        Data = Data.substring(Data2.length(), Data.length());
      else
        Data = "";
    } else if (Fn == 241 || Fn == 243) {  //AA+Fn+Sub+CH+IO+Val+CRC(2b)
      Data2 = Data.substring(0, 16);

      if (Data.length() > Data2.length())
        Data = Data.substring(Data2.length(), Data.length());
      else
        Data = "";
    } else if (Fn == 6) {  //AA+Fn+Sub+CH+Val(2b)+CRC(2b)
      Data2 = Data.substring(0, 16);

      if (Data.length() > Data2.length())
        Data = Data.substring(Data2.length(), Data.length());
      else
        Data = "";
    } else {
      Data2 = Data;
      delay(1);
      Data = "";
    }

    if (Tools__ModbusCRC(Data2.substring(0, Data2.length() - 4)) == Data2.substring(Data2.length() - 4, Data2.length())) {  //Check CRC

      if (SOFTWARE_DEBUGMODE) {
        Debug_ConsolePrint("AA = ");
        Debug_ConsolePrintln(String(AA));
        Debug_ConsolePrint("Fn = ");
        Debug_ConsolePrintln(String(Fn));
        Debug_ConsolePrint("Sub = ");
        Debug_ConsolePrintln(String(Sub));
        Debug_ConsolePrint("CH = ");
        Debug_ConsolePrintln(String(CH));
      }
      if (Fn == 241) {                                             //HEX=F1 Decimal=241
        byte val = Tools__HexToDecimal(Data.substring(10, 12));  //Lock=255 & Unlock=0
        if (CH == 255) {
          for (byte i = 0; i < RELAY_MAX; i++) {
            if (val == 255) {
              MemoWriteByte(EP_RELAY_LOCK_S + i, 1);
              _Dvc_Relay[i].Lock = 1;
            } else if (val == 0) {
              MemoWriteByte(EP_RELAY_LOCK_S + i, 0);
              _Dvc_Relay[i].Lock = 0;
            }
          }

        } else {
          for (byte i = 0; i < RELAY_MAX; i++)
            if (AA == _Dvc_Relay[i].Address && Sub == _Dvc_Relay[i].Subnet && CH == _Dvc_Relay[i].Channel) {
              if (val == 255) {
                MemoWriteByte(EP_RELAY_LOCK_S + i, 1);
                _Dvc_Relay[i].Lock = val;
              } else if (val == 0) {
                MemoWriteByte(EP_RELAY_LOCK_S + i, 0);
                _Dvc_Relay[i].Lock = val;
              }
              if (IO == 0) {
                Data = Data.substring(0, 8) + "11" + Data.substring(10, 12);
                Data.toUpperCase();
                Data += Tools__ModbusCRC(Data);
                Tools__ModbusSend(Data2, "Auto");  //Response
              }
            }
        }
      }
      //-----------------------------------------------
      if (Fn == 243) {  //Check system online      HEX=F3  Decimal=243
        for (byte i = 0; i < RELAY_MAX; i++)
          if (AA == _Dvc_Relay[i].Address && Sub == _Dvc_Relay[i].Subnet && CH == _Dvc_Relay[i].Channel && IO == 0) {
            Data2 = Data2.substring(0, 8) + "11" + Data2.substring(10, 12);
            Data2.toUpperCase();
            Data2 += Tools__ModbusCRC(Data2);
            Tools__ModbusSend(Data2, "Auto");  //Response
          }
        //        return;
      }
      //-----------------------------------------------
      else if (Fn == 244 && CH == 0) {  //ON or OFF Channels
        //    Serial1.println("F4");
        bool DoIT = false;
        byte Len = (Tools__HexToDecimal(Data2.substring(10, 12))) * 2;  //Size of Mode+value+Data
        byte Mode = Tools__HexToDecimal(Data2.substring(12, 14));       //Mode
        byte val = Tools__HexToDecimal(Data2.substring(14, 16));        //value
        if (Mode == 1) {
          for (byte i = 16; i < 12 + Len; i += 2) {
            byte CH1 = Tools__HexToDecimal(Data2.substring(i, i + 2));
            //        Serial1.println(CH1);
            for (byte z = 0; z < RELAY_MAX; z++) {
              if (AA != _Dvc_Relay[z].Address || Sub != _Dvc_Relay[z].Subnet || CH1 != _Dvc_Relay[z].Channel || _Dvc_Relay[z].Lock != 0)
                continue;

              byte TC = 0;
              DoIT = true;
              switch (_Dvc_Relay[z].Type) {
                case 0:
                  if (val == 255) {
                    _Dvc_Led[z] = 1;
                    Dvc__RelayControl(z);
                  } else if (val == 0) {
                    _Dvc_Led[z] = 0;
                    Dvc__RelayControl(z);
                  }
                  break;
                case 2:
                  TC = 2;
                  if (val == 0) {
                    _Dvc_Led[z] = 0;
                    _Dvc_Led[z + 1] = 0;
                  } else if (val == 255) {
                    _Dvc_Led[z] = 1;
                    _Dvc_Led[z + 1] = 0;
                  }
                  Dvc__RelayControl(z);
                  break;
                case 3:
                  TC = 3;
                  if (val == 0) {
                    _Dvc_Led[z] = 0;
                    _Dvc_Led[z + 1] = 0;
                    _Dvc_Led[z + 2] = 0;
                  } else if (val == 255) {
                    _Dvc_Led[z] = 1;
                    _Dvc_Led[z + 1] = 0;
                    _Dvc_Led[z + 2] = 0;
                  }
                  Dvc__RelayControl(z);
                  break;
              }
              Dvc__SendData(1, z, TC);
              delay(150);
            }
          }
        }
        if (Mode == 2) {
          for (byte i = 16; i < 12 + Len; i += 4) {
            byte CH_Low = Tools__HexToDecimal(Data2.substring(i, i + 2));
            byte CH_High = Tools__HexToDecimal(Data2.substring(i + 2, i + 4));
            for (byte j = CH_Low; j <= CH_High; j++) {
              for (byte z = 0; z < 10; z++) {
                if (AA == _Dvc_Relay[z].Address && Sub == _Dvc_Relay[z].Subnet && j == _Dvc_Relay[z].Channel && _Dvc_Relay[z].Lock == 0) {
                  byte TC = 0;
                  DoIT = true;
                  switch (_Dvc_Relay[z].Type) {
                    case 0:
                      if (val == 255) {
                        _Dvc_Led[z] = 1;
                        Dvc__RelayControl(z);
                      } else if (val == 0) {
                        _Dvc_Led[z] = 0;
                        Dvc__RelayControl(z);
                      }
                      break;
                    case 2:
                      TC = 2;
                      if (val == 0) {
                        _Dvc_Led[z] = 0;
                        _Dvc_Led[z + 1] = 0;
                      } else if (val == 255) {
                        _Dvc_Led[z] = 1;
                        _Dvc_Led[z + 1] = 0;
                      }
                      Dvc__RelayControl(z);
                      break;
                    case 3:
                      TC = 3;
                      if (val == 0) {
                        _Dvc_Led[z] = 0;
                        _Dvc_Led[z + 1] = 0;
                        _Dvc_Led[z + 2] = 0;
                      } else if (val == 255) {
                        _Dvc_Led[z] = 1;
                        _Dvc_Led[z + 1] = 0;
                        _Dvc_Led[z + 2] = 0;
                      }
                      Dvc__RelayControl(z);
                      break;
                  }
                  Dvc__SendData(1, z, TC);
                  delay(150);
                }
              }
            }
          }
        }




        if (DoIT == false)
          Check_CustomCode(Data2);
      }
      //-----------------------------------------------
      else if (Fn == 242) {  //HEX=F2  Decimal=242
        if (AA == 254 && Sub == 0 && CH == 252) {
          byte Val = Tools__HexToDecimal(Data2.substring(14, 16));
          Check_VirtualCode(Val);
        } else {
          byte i = 0;
          for (; i < RELAY_MAX; i++)
            if (AA == _Dvc_Relay[i].Address && Sub == _Dvc_Relay[i].Subnet && CH == _Dvc_Relay[i].Channel) break;
          if (i >= RELAY_MAX)
            Check_CustomCode(Data2);
          else {

            byte TC = 0;
            String val = Data2.substring(12, 16);
            switch (_Dvc_Relay[i].Type) {
              case T_Light:
              case T_Siren:

                if (_Dvc_Relay[i].Type == T_RemoteRelay && IO == 0)
                  Dvc__SendData(1, i, TC);
                if (val == "00FF") {
                  _Dvc_Led[i] = 1;
                  Dvc__RelayControl(i);
                } else if (val == "0000") {
                  _Dvc_Led[i] = 0;
                  Dvc__RelayControl(i);
                }
                break;
              case T_RemoteRelay:
                if (val == "0000") {
                  _Dvc_Led[i] = 0;
                  W_RelayOFF[i] = 1;
                  Button__LEDShow();
                  delay(40);
                  W_RelayOFF[i] = 0;
                  Button__LEDShow();
                } else Dvc__RelayControl(i);
                break;
            }
            if (IO == 0)
              Dvc__SendData(1, i, TC);
          }
        }
      }
      //-----------------------------------------------
      else if (Fn == 240) {  //HEX=F0  Decimal=240
        byte Type = Tools__HexToDecimal(Data2.substring(8, 10));
        IO = Tools__HexToDecimal(Data2.substring(10, 12));
        if (SOFTWARE_DEBUGMODE) {
          Debug_ConsolePrint("Type = ");
          Debug_ConsolePrintln(String(Type));
          Debug_ConsolePrint("IO = ");
          Debug_ConsolePrintln(String(IO));
        }
        if ((Type == 0 || Type == 1) && IO == 0 && Data2.substring(14, 16) == "14") {
          if (SOFTWARE_DEBUGMODE) {
            Debug_ConsolePrint("SerialBarcodeRead = ");
            Debug_ConsolePrintln(Tools__SerialBarcodeRead());
            Debug_ConsolePrint("Data.substring(18, 38) = ");
            Debug_ConsolePrintln(Data.substring(18, 38));
          }
          if (Tools__SerialBarcodeRead() += Data2.substring(18, 38)) {
            return;
            byte BT = Tools__HexToDecimal(Data2.substring(16, 18));
            //      Serial.print("BT= "); Serial.println(BT);
            switch (BT) {
              case 1:
                BT = 0;
                break;
              case 2:
                BT = 5;
                break;
              case 3:
                BT = 7;
                break;
              case 4:
                BT = 6;
                break;
            }
            switch (Type) {
              case 0:
                if (_Dvc_Relay[BT].Address == 255 || _Dvc_Relay[BT].Channel == 255) {
                  MemoWriteByte(EP_RELAY_ADDRESS_S + BT, AA);
                  MemoWriteByte(EP_RELAY_SUBNET_S + BT, Sub);  //Save Subnnet To EEPROM
                  MemoWriteByte(EP_RELAY_CHANNEL_S + BT, CH);  //Save Channel To EEPROM
                  _Dvc_Relay[BT].Address = AA;
                  _Dvc_Relay[BT].Subnet = Sub;
                  _Dvc_Relay[BT].Channel = CH;
                }  //else return;
                break;
            }
          }
        } else if (Data2.substring(14, 16) == "04") {  //return serial to server
          byte i = 0;
          for (; i < 10; i++) {
            if (AA == _Dvc_Relay[i].Address && Sub == _Dvc_Relay[i].Subnet && CH == _Dvc_Relay[i].Channel) break;
          }
          if (i < 10) {
            String SerialLable = Data2.substring(0, 8) + "110B04" + Tools__SerialBarcodeRead();
            // ADM__SendData_2(SerialLable);
          }
        }
        if (AA != 1 || Sub != 255 || CH != 255 || Type != 255 || IO != 0) {  //Check Addres & Subnet & Channel

          byte Len = Tools__HexToDecimal(Data2.substring(12, 14)) * 2;
          byte Mode = Tools__HexToDecimal(Data2.substring(14, 16));
          byte Var = Tools__HexToDecimal(Data2.substring(16, 18));
          //    Serial.print("Mode= "); Serial.println(Mode);
          if (Mode == 0) {                                     //Request for detect Device
            // ADM__SendData_2("01F0FFFFFF110400" + _Dvc_TypeCode);  //send Type(02)(Key) & Var(04)(4Gang)
          } else if (Mode == 1 && _Dvc_HasBarcode == false) {  //Save Serial _Dvc_HasBarcode
            String sentence = Data2.substring(16, 14 + Len);
            delay(3);
            if (sentence.substring(0, 6) != _Dvc_TypeCode) return;
            for (byte i = 1; i <= Len - 2; i += 2) {
              byte j = Tools__HexToDecimal(sentence.substring(i - 1, i + 1));
              MemoWriteByte(EP_SERIAL_S + ((i + 1) / 2) - 1, j);
            }

            _Dvc_HasBarcode = true;
            // ADM__SendData_2("01F0FFFFFF11" + Data2.substring(12, Data2.length() - 4));


            for (byte i = 0; i <= 6; i++) {
              MemoWriteByte(EP_RELAY_ADDRESS_S + i, 255);
              MemoWriteByte(EP_RELAY_SUBNET_S + i, 255);   //Save Subnnet To EEPROM
              MemoWriteByte(EP_RELAY_CHANNEL_S + i, 255);  //Save Channel To EEPROM
              _Dvc_Relay[i].Address = 255;
              _Dvc_Relay[i].Subnet = 255;
              _Dvc_Relay[i].Channel = 255;
            }
            for (byte i = 0; i < 6; i++) {
              for (byte i = 0; i < 9; i++) {
                _Dvc_Led[i] = !_Dvc_Led[i];
              }
              Button__LEDShow();
              delay(200);
            }
          } else if (Mode == 2) {  //Request for send Serial to system
            String SerialLable = "";
            // ADM__SendData_2("01F0FFFFFF110B02" + Tools__SerialBarcodeRead());  //Send Serial to System
          } else if (Mode == 17) {  //Request DevSerch
            //if (_Dvc_Phase == 0)
            //   ADM__SendData_2("01F0FFFFFF110B11" + Tools__SerialBarcodeRead());  //Send Serial to System
          } else if (Mode == 18) {  //Save DevSerch
            if (Data2.substring(18, 38) != Tools__SerialBarcodeRead()) return;
            _Dvc_Phase = Var;
          } else if (Mode == 19) {  //I'm Here (Blinker)
            if (Data2.substring(16, 36) != Tools__SerialBarcodeRead()) return;

            for (byte i = 0; i < 6; i++) {
              for (byte i = 0; i < 9; i++) {
                _Dvc_Led[i] = !_Dvc_Led[i];
              }
              Button__LEDShow();
              delay(200);
            }
          }
        }
      }
    } else
      Check_CustomCode(Data2);
  }
}


void Dvc__RelayControl(byte L) {

  if (_Dvc_Relay[L].Type == T_Light || _Dvc_Relay[L].Type == T_Siren) {
    if (_Dvc_Led[L] == 1) {
      W_RelayON[L] = 1;
      Button__LEDShow();
      delay(40);
      W_RelayON[L] = 0;
      Button__LEDShow();
    } else {
      W_RelayOFF[L] = 1;
      Button__LEDShow();
      delay(40);
      W_RelayOFF[L] = 0;
      Button__LEDShow();
    }
  } else if (_Dvc_Relay[L].Type == T_RemoteRelay) {
    W_RelayON[L] = 1;
    _Dvc_Led[L] = 1;
    Button__LEDShow();
    delay(40);
    W_RelayON[L] = 0;
    Button__LEDShow();
    _Dvc_RelayTimeout[L] = _Dvc_Relay[L].Hold;
  }
  MemoWriteByte(EP_LASTDVCMODE_S + L, _Dvc_Led[L]);
}


void Dvc__SendData(byte TypeCircuit, byte LineNumber, byte TypeCode) {
  //SendSettingToAllOnMesh();
  String ADD = Tools__ByteToHexstring(_Dvc_Relay[LineNumber].Address);
  String Sub = Tools__ByteToHexstring(_Dvc_Relay[LineNumber].Subnet);
  String CH = Tools__ByteToHexstring(_Dvc_Relay[LineNumber].Channel);
  String Value = "";
  byte CLine, FLine;
  switch (TypeCode) {
    case T_Light:
    case T_Siren:
      if (_Dvc_Led[LineNumber] == 1) Value = "00FF";
      else Value = "0000";
      break;

    case T_RemoteRelay:
      Value = "00FF";
      break;
  }

  String Data = ADD + "F2" + Sub + CH + "1102" + Value;  //AA + Fn + sub + CH + IO + Len + val
  Data += Tools__ModbusCRC(Data);
  Data.toUpperCase();
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("\r\nDvc__SendData Prepare... " + Data);
  Tools__ModbusSend(Data, TypeCircuit == 0 ? "Manual" : "Auto");
}

