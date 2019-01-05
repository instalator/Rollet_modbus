void checkSensor() {
  if (PIND & (1 << PD2) == 0) { //Если сработал датчик Zerro, отключаем выходы
    rollet.disableOutputs();
  }
  if ((PINB & (1 << PB0)) == 0 && (PIND & (1 << PD2)) != 0) { //Если сработал датчик Mode и шторы не в нуле
    if (!rollet.isRunning()) { // Если мотор не запущен то входим в режим программирования
      LearningMode();
    } else { //Если мотор запущен, то останавливаем его (аварийка)
      position = rollet.currentPosition();
      rollet.setAcceleration(speed);
      rollet.stop();
      rollet.disableOutputs();
    }
  }
}

double GetTemp(void) {
  unsigned int wADC;
  double t;
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN);  // enable the ADC
  delay(20);            // wait for voltages to become stable.
  ADCSRA |= _BV(ADSC);  // Start the ADC
  while (bit_is_set(ADCSRA, ADSC));
  // Reading register "ADCW" takes care of how to read ADCL and ADCH.
  wADC = ADCW;
  // The offset of 324.31 could be wrong. It is just an indication.
  t = (wADC - 324.31 ) / 1.22;
  return (t);
}

int availableMemory() {
  int size = 2048;
  byte *buf;
  while ((buf = (byte *) malloc(--size)) == NULL);
  free(buf);
  return size;
}
