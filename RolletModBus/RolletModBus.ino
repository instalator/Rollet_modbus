#include<AccelStepper.h>
#include <EEPROM.h>
#include <avr/wdt.h>
#include <ModbusRtu.h>

/*********** КОНФИГИ ************/
#define MOD_ID  2  //Modbus адрес
/********************************/
#define FOTORES  A0 // Фоторезистор освещенность
#define IRSENSE  A1 // Фоторезистор импульсный
#define WINDOW 9  // Геркон открытия окна
#define LED 10 // Светодиод
#define ZERRO 2 //Датчик нуля
#define MODE 8 //Датчик холла Mode
#define UP -250000
// Адреса ячеек EEPROM для хранения данных
#define DIST_ADR  10
#define RIGHT_ADR 15
#define SPEED_ADR 20
#define ACCEL_ADR 30
#define LED_ADR 40
#define ID_ADR 42
#define CHKPOS_ADR 44
#define BAUD_ADR 50

AccelStepper rollet(8, 7, 5, 6, 4); //right /P1-4 IN1/P2-5 IN2/P3-6 IN3/P4-7 IN4/
//AccelStepper rolletl(8, 4, 6, 5, 7); //left

Modbus slave(MOD_ID, 0, 3); // (id, Serial, PIN)
void (* resetFunc) (void) = 0;

byte id = MOD_ID; //Modbus адрес
int speed = 800; //Скорость мотора
int acceleration = 200; //Ускорение мотора
long position = 1;
long dist;
long current = 0;
unsigned long timer;
bool learn = false;
bool right = true;
bool check_pos = false;
bool led = true;
bool firststart = true;
bool check_pos_flag = false; //Разрешение установки текущего положени после подачи питания
int8_t state = 0;
int pos = 0;
int pre_pos = 0;
uint16_t au16data[15];
byte baud = 5;
long baudrate[12] = {300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200};

void setup() {
  MCUSR = 0;
  wdt_disable();
  pinMode(3, OUTPUT); //MAX485
  pinMode(4, OUTPUT); //P1
  pinMode(5, OUTPUT); //P2
  pinMode(6, OUTPUT); //P3
  pinMode(7, OUTPUT); //P4
  pinMode(LED, OUTPUT);
  pinMode(ZERRO, INPUT_PULLUP); // Датчик холла
  pinMode(MODE, INPUT_PULLUP); // Конфигурационный Датчик холла
  for (int i = 0; i < 10; i++) {
    digitalWrite(LED, !digitalRead(LED));
    delay(100);
  }
  eepromRW();
  slave.begin(baudrate[baud]);
  slave.setID(id); //Устанавливаем ID устройства Modbus
  if (!right) {
    //AccelStepper rollet(8, 4, 6, 5, 7);
    //AccelStepper::AccelStepper(8, 4, 6, 5, 7);
  }
  bitWrite(au16data[0], 3, led);
  bitWrite(au16data[0], 4, 0);
  bitWrite(au16data[0], 5, right);
  bitWrite(au16data[0], 6, check_pos_flag);
  bitWrite(au16data[0], 7, 0);
  au16data[6] = acceleration;
  au16data[7] = speed;
  au16data[8] = id;
  au16data[9] = (rollet.currentPosition() * 100) / dist;
  au16data[11] = baud + 1;
  rollet.setMaxSpeed(speed);
  rollet.setAcceleration(acceleration);
  attachInterrupt(0, Stop, FALLING);
  digitalWrite(LED, led);
  if (check_pos_flag) {
    firststart = false;
    CheckPosOnPower();
  }
  wdt_enable(WDTO_8S);
}

void loop() {
  wdt_reset();
  state = slave.poll( au16data, 15);
  bitWrite(au16data[0], 7, rollet.isRunning());
  if (state > 4) {
  }
  if (!rollet.isRunning()) {
    io_poll();
  } else {
    pos = au16data[9];
    au16data[10] = (rollet.currentPosition() * 100) / dist;
  }
  checkSensor();
  if (pos != pre_pos) {
    pre_pos = pos;
    if (pos >= 0 && pos <= 100) {
      if (rollet.isRunning()) {
        rollet.setAcceleration(speed);
        rollet.stop();
      }
      if(firststart && !check_pos_flag){ //Если запуск после подачи питания и нет флага проверки текущего положения
        CheckPosOnPower();
      } else {
        position = (dist * pos) / 100;
      }
    }
  }
  rollet.moveTo(position);
  rollet.run();
  if (rollet.distanceToGo() == 0) {
    rollet.disableOutputs();
  }
}



