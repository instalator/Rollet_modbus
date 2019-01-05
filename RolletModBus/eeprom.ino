void eepromRW() {
  if (EEPROM.read(1) != 99) { //Если первый запуск
    EEPROM.write(1, 99);
    EEPROM.write(RIGHT_ADR, right);
    EEPROMWriteInt(SPEED_ADR, speed);
    EEPROMWriteInt(ACCEL_ADR, acceleration);
    EEPROM.write(LED_ADR, led);
    EEPROM.write(ID_ADR, id);
    EEPROM.write(CHKPOS_ADR, check_pos_flag);
    EEPROM.write(BAUD_ADR, baud);
    resetFunc();
  } else {
    dist = EEPROMReadLong(DIST_ADR);
    right = EEPROM.read(RIGHT_ADR);
    speed = EEPROMReadInt(SPEED_ADR);
    acceleration = EEPROMReadInt(ACCEL_ADR);
    led = EEPROM.read(LED_ADR);
    id  = EEPROM.read(ID_ADR);
    check_pos_flag = EEPROM.read(CHKPOS_ADR);
    baud = EEPROM.read(BAUD_ADR);
  }
}

void Save() {
  EEPROM.write(1, 99);
  EEPROM.write(RIGHT_ADR, right);
  EEPROMWriteInt(SPEED_ADR, speed);
  EEPROMWriteInt(ACCEL_ADR, acceleration);
  EEPROM.write(LED_ADR, led);
  EEPROM.write(ID_ADR, id);
  EEPROM.write(CHKPOS_ADR, check_pos_flag);
}

void EEPROMWriteLong(int p_address, unsigned long p_value) {
  byte four = (p_value & 0xFF);
  byte three = ((p_value >> 8) & 0xFF);
  byte two = ((p_value >> 16) & 0xFF);
  byte one = ((p_value >> 24) & 0xFF);
  EEPROM.write(p_address, four);
  EEPROM.write(p_address + 1, three);
  EEPROM.write(p_address + 2, two);
  EEPROM.write(p_address + 3, one);
}

unsigned long EEPROMReadLong(int p_address) {
  long four = EEPROM.read(p_address);
  long three = EEPROM.read(p_address + 1);
  long two = EEPROM.read(p_address + 2);
  long one = EEPROM.read(p_address + 3);
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void EEPROMWriteInt(int p_address, int p_value) {
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);
  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);
}
unsigned int EEPROMReadInt(int p_address) {
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);
  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}
