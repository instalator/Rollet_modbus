void io_poll() {
  bitWrite(au16data[0], 0, digitalRead(WINDOW));
  bitWrite(au16data[0], 1, !digitalRead(ZERRO));
  bitWrite(au16data[0], 2, !digitalRead(MODE));
  if (bitRead(au16data[0], 3) != led) {
    led = bitRead(au16data[0], 3);
    digitalWrite(LED, led);
    EEPROM.write(LED_ADR, led);
  }
  if (bitRead(au16data[0], 4) == 1) {
    bitWrite(au16data[0], 4, 0);
    if (!learn) {
      LearningMode();
    }
  }
  if (bitRead(au16data[0], 5) != right) {
    right = bitRead(au16data[0], 5);
    EEPROM.write(RIGHT_ADR, right);
    resetFunc();
  }
  if (bitRead(au16data[0], 6) != check_pos_flag) {
    check_pos_flag = bitRead(au16data[0], 6);
    EEPROM.write(CHKPOS_ADR, check_pos_flag);
  }
  bitWrite(au16data[0], 7, rollet.isRunning());
  au16data[2] = analogRead(FOTORES);
  au16data[3] = analogRead(IRSENSE);
  au16data[4] = GetTemp();
  au16data[5] = availableMemory();
  if (au16data[6] != acceleration) {
    acceleration = au16data[6];
    EEPROMWriteInt(ACCEL_ADR, acceleration);
  }
  if (au16data[7] != speed) {
    speed = au16data[7];
    EEPROMWriteInt(SPEED_ADR, speed);
  }
  if (au16data[8] != id ) {
    id = au16data[8];
    slave.setID(id); //Устанавливаем ID устройства на шине
    EEPROM.write(ID_ADR, id);
  }
  pos = au16data[9]; // TODO объединить текущее положение и установку
  au16data[10] = (rollet.currentPosition() * 100) / dist;
  if (au16data[11] != baud + 1) {
    if (au16data[11] > 0 && au16data[11] <= 12) {
      baud = au16data[11] - 1;
      EEPROM.write(BAUD_ADR, baud);
      resetFunc();
    }
  }
  au16data[12] = slave.getInCnt();
  au16data[13] = slave.getOutCnt();
  au16data[14] = slave.getErrCnt();
}
