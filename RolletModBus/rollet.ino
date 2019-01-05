void CheckPosOnPower() { //Zerro mode, проверка текущего положения при повторной подачи питания
  if (PIND & (1 << PD2) != 0) {
    check_pos = true;
    rollet.setCurrentPosition(0);
    rollet.setMaxSpeed(speed);
    rollet.setAcceleration(acceleration);
    position = UP;
  }
}

void LearningMode() { //Programming mode!
  unsigned long prevm;
  int i;
  learn = true;
  dist = 0;
  rollet.setCurrentPosition(0);
  rollet.setMaxSpeed(speed);
  rollet.setAcceleration(acceleration);
  rollet.moveTo(UP);
  bitWrite(au16data[0], 4, 1);
  while (rollet.currentPosition() != UP) {
    wdt_reset();
    bitWrite(au16data[0], 7, rollet.isRunning());
    state = slave.poll( au16data, 15);
    if (dist == 0) {
      rollet.run();
      if (millis() - prevm > 200) { // Если в течении 200ms * 20 датчик mode в нуле - сброс на заводские
        prevm = millis();
        digitalWrite(LED, !digitalRead(LED));
        if ((PINB & (1 << PB0)) == 0) { //Если датчик mode сработал
          i++;
          if (i >= 20) { //Сброс на заводские настройки
            EEPROM.write(1, 0);
            resetFunc();
          }
        }
      }
      if (millis() - prevm > 5000) {
        prevm = millis();
        i = 0;
      }
    } else {
      digitalWrite(LED, led);
      break;
    }
  }
  rollet.setCurrentPosition(0);
  position = dist;
  EEPROMWriteLong(DIST_ADR, dist);
  bitWrite(au16data[0], 4, 0);
  learn = false;
}

void Stop() {
  detachInterrupt(0);
  if (rollet.isRunning()) {
    rollet.setAcceleration(speed);
    rollet.stop();
    rollet.disableOutputs();
    if (check_pos) {
      check_pos = false;
      position = rollet.currentPosition() * -1;
      rollet.setCurrentPosition(0);
    } else if (!check_pos && !learn) {
      rollet.setCurrentPosition(0);
      position = 0;
    }
    if (learn) {
      dist = rollet.currentPosition() * -1;
    }
    if (firststart){
        firststart = false;
        position = (dist * pos) / 100;
    }
    rollet.setMaxSpeed(speed);
    rollet.setAcceleration(acceleration);
    rollet.disableOutputs();
  }
  attachInterrupt(0, Stop, FALLING);
}
