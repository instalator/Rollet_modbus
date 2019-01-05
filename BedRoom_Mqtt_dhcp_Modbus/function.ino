void shiftqueue() {
  for (int i = 3; i < TELEGRAMSIZE - 1; i++) {
    if (telegram[i].u8id == 0 && telegram[i + 1].u8id != 0) {
      telegram[i] = telegram[i + 1];
      telegram[i + 1].u8id = 0;
    }
  }
}

void SenCmd() {
  if (master.getState() == COM_IDLE) {
    if (telegram[3].u8id != 0) {
      master.query(telegram[3]);
      telegram[3].u8id = 0;
      u8state = 0;
      u8query = 0;
      u32wait = millis() + 1000;
    }
  }
}

String topic(String s) {
  return (TOPIC + s);
}

boolean reconnect() {
  if (mqtt.connect(ID_CONNECT)) {
    wdt_reset();
    char s[16];
    sprintf(s, "%d.%d.%d.%d", Ethernet.localIP()[0], Ethernet.localIP()[1], Ethernet.localIP()[2], Ethernet.localIP()[3]);
    mqtt.publish(topic("ip").c_str(), s);
    mqtt.publish(topic("connection").c_str(), "true");
    getSensors();
    PubTopic();
    mqtt.subscribe(topic("#").c_str());
  }
  return mqtt.connected();
}

void checkmqtt() {
  if (!mqtt.loop()) {
    if (millis() - prevMillis > 60000) {
      prevMillis = millis();
      if (Ethernet.linkStatus() == LinkON) { //Если есть линк
        if (Ethernet.begin(mac) == 0) {
          Reset();
        } else {
          reconnect();
        }
      }
    }
  }
}

void mqttSetup() {
  /*int idcheck = EEPROM.read(0);
    if (idcheck == IDS) {
    for (int i = 0; i < 4; i++) {
      mqtt_serv[i] = EEPROM.read(i + 7);
    }
    }*/
  mqtt.setServer(mqtt_serv, 1883);
  mqtt.setCallback(callback);
  reconnect();
}

void EthernetSetup() {
  //Ethernet.setRetransmissionTimeout(0x01F4);
  //Ethernet.setRetransmissionCount(3);
  /*int idcheck = EEPROM.read(0);
    if (idcheck == IDS) {
    for (int i = 0; i < 6; i++) {
      mac[i] = EEPROM.read(i + 1);
    }
    }*/
  Ethernet.begin(mac);
}

void getSensors() {
  Window = digitalRead(5);
  Motion = digitalRead(4);
  if (DHT.read22(DHT22_PIN) == DHTLIB_OK) {
    if (DHT.humidity != 0 || DHT.temperature != 0) {
      Hout = DHT.humidity;
      Tout = DHT.temperature;
    }
  }
  DS_sensors.requestTemperatures();
  
  if (Window != old_Window) {
    old_Window = Window;
    mqtt.publish(topic("Window").c_str(), BoolToChar(Window));
  }
  if (Motion != old_Motion) {
    old_Motion = Motion;
    mqtt.publish(topic("Motion").c_str(), BoolToChar(Motion));
  }
}

bool SrtToLvl(String st) {
  if (st == "false" || st == "0" || st == "off") {
    return 0;
  } else {
    return 1;
  }
}

const char* BoolToChar (bool r) {
  return r ? "true" : "false";
}
const char* IntToChar (unsigned int v) {
  sprintf(buffer, "%d", v);
  return buffer;
}

const char* FloatToChar (float f) {
  sprintf(buffer, "%d.%02d", (int)f, (unsigned int)(f * 100) % 100);
  return buffer;
}

void Reset() {
  for (;;) {}
}
