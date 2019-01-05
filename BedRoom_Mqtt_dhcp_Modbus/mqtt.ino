void callback(char* topic, byte* Payload, unsigned int length) {
  Payload[length] = '\0';
  String top = String(topic);
  String payload = String((char*)Payload);
  uint8_t numw = top.substring(top.indexOf("w") + 1, top.indexOf("w") + 2).toInt();
  String cmd = top.substring(top.lastIndexOf("/") + 1);
  int reg = 0;

  if (cmd.equals("led")) {
    bitWrite( data[numw - 1][0], 3, SrtToLvl(payload));
    //reg = 0;
  }
  /////////////////////////////////////////////////////////////////////////
  else if (cmd.equals("lean_mode")) {
    bitWrite( data[numw - 1][0], 4, SrtToLvl(payload));
    //reg = 0;
  }
  /////////////////////////////////////////////////////////////////////////
  else if (cmd.equals("lean_mode")) {
    bitWrite( data[numw - 1][0], 4, SrtToLvl(payload));
    //reg = 0;
  }
  /////////////////////////////////////////////////////////////////////////
  else if (cmd.equals("right")) {
    bitWrite( data[numw - 1][0], 5, SrtToLvl(payload));
    //reg = 0;
  }
  /////////////////////////////////////////////////////////////////////////
  else if (cmd.equals("zerro_flag")) {
    bitWrite( data[numw - 1][0], 6, SrtToLvl(payload));
    //reg = 0;
  }
  /////////////////////////////////////////////////////////////////////////
  else if (cmd.equals("acclr")) {
    data[numw - 1][6] = payload.toInt();
    reg = 6;
  }
  /////////////////////////////////////////////////////////////////////////
  else if (cmd.equals("speed")) {
    data[numw - 1][7] = payload.toInt();
    reg = 7;
  }
  /////////////////////////////////////////////////////////////////////////
  else if (cmd.equals("id")) {
    data[numw - 1][8] = payload.toInt();
    reg = 8;
  }
  /////////////////////////////////////////////////////////////////////////
  else if (cmd.equals("set_pos")) {
    data[numw - 1][9] = payload.toInt();
    reg = 9;
  }
  /////////////////////////////////////////////////////////////////////////
  else if (cmd.equals("baud")) {
    data[numw - 1][11] = payload.toInt();
    reg = 11;
  }
  /////////////////////////////////////////////////////////////////////////
  else if (cmd.equals("Servo")) {
    ServoAngle = payload.toInt();
    if (ServoAngle >= 0 && ServoAngle <= 255) {
      servo.attach(SERVO_PIN);
      servo.write(ServoAngle); //ставим вал под 0
      delay(1000);
      servo.detach();
      return;
    }
  }
  /////////////////////////////////////////////////////////////////////////
  for (int i = 4; i < 7; i++) {
    if (telegram[i].u8id == 0) {
      telegram[i].u8id = numw;
      //telegram[i].u8fct = 6;
      telegram[i].u16RegAdd = reg;
      //telegram[i].u16CoilsNo = 1;
      telegram[i].au16reg = data[numw - 1] + reg;
      break;
    }
  }
}

void PubTopic () {
  if (mqtt.connected()) {
    for (int i = 0; i < NUMWINDOW; i++) {
      String t = TOPIC + "w";
      t += i + 1;
      t +="/";
      //mqtt.publish((t + "window").c_str(), BoolToChar(bitRead( data[i][0], 0 )));
      mqtt.publish((t + "zerro").c_str(), BoolToChar(bitRead( data[i][0], 1 )));
      mqtt.publish((t + "mode").c_str(), BoolToChar(bitRead( data[i][0], 2 )));
      mqtt.publish((t + "led").c_str(), BoolToChar(bitRead( data[i][0], 3 )));
      mqtt.publish((t + "lean_mode").c_str(), BoolToChar(bitRead( data[i][0], 4 )));
      //mqtt.publish((t + "right").c_str(), BoolToChar(bitRead( data[i][0], 5 )));
      mqtt.publish((t + "zerro_flag").c_str(), BoolToChar(bitRead( data[i][0], 6 )));
      mqtt.publish((t + "is_running").c_str(), BoolToChar(bitRead( data[i][0], 7 )));
      //mqtt.publish((t + "fotores").c_str(), IntToChar(data[i][2]));
      //mqtt.publish((t + "irsense").c_str(), IntToChar(data[i][3]));
      mqtt.publish((t + "temp").c_str(), IntToChar(data[i][4]));
      //mqtt.publish((t + "mem").c_str(), IntToChar(data[i][5]));
      mqtt.publish((t + "acclr").c_str(), IntToChar(data[i][6]));
      mqtt.publish((t + "speed").c_str(), IntToChar(data[i][7]));
      mqtt.publish((t + "id").c_str(), IntToChar(data[i][8]));
      mqtt.publish((t + "set_pos").c_str(), IntToChar(data[i][10]));
      mqtt.publish((t + "cur_pos").c_str(), IntToChar(data[i][10]));
      mqtt.publish((t + "baud").c_str(), IntToChar(data[i][11]));
    }
    mqtt.publish(topic("Temp_room").c_str(), FloatToChar(Tout));
    mqtt.publish(topic("Humidity_room").c_str(), FloatToChar(Hout));
    mqtt.publish(topic("Temp_radiator").c_str(), FloatToChar(DS_sensors.getTempCByIndex(1)));
    mqtt.publish(topic("Temp_in").c_str(), FloatToChar(DS_sensors.getTempCByIndex(0)));
    mqtt.publish(topic("Lux").c_str(), IntToChar(analogRead(FOTO_PIN)));
    mqtt.publish(topic("Servo").c_str(), IntToChar(ServoAngle));
    mqtt.publish(topic("MQ7").c_str(), IntToChar(analogRead(MQ7_PIN)));
  }
}
