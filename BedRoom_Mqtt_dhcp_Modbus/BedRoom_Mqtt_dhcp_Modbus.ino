#include <SPI.h>           // Ethernet shield
#include <Ethernet.h>      // >= 2.0.0 Ethernet shield
#include <PubSubClient.h>  // MQTT 
#include <ModbusRtu.h>
#include <TextFinder.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <Servo.h>
#include <dht.h>
#include <OneWire.h>
#include <DallasTemperature.h>

String TOPIC = "myhome/Bedroom/";
#define ID_CONNECT   "bedroom"
#define IDS 91
#define NUMWINDOW 3 // Количество штор
#define TELEGRAMSIZE 7 //Размер массива modbus_t telegram[TELEGRAMSIZE];
#define BAUD 9600

//#define HTTP_SERVER
#define SERVO_PIN    3 //Порт к которому подключен сервопривод
#define ONE_WIRE_BUS 8 //Шина данных датчика DS18B20
#define DHT22_PIN    9 //Шина данных датчика DHT22
#define MQ7_PIN      A0 // Вход подключения MQ7
#define FOTO_PIN     A1 // Вход для Фоторезистора

byte mac[6]  = { 0xDE, 0xAD, 0x2E, 0x41, 0xE2, 0x14 }; //MAC адрес контроллера
byte mqtt_serv[4] = {192, 168, 88, 190}; //IP MQTT брокера

Modbus master(0, 0, 16); // this is master ID-0/Serial/PIN A2 for MAX485
EthernetClient ethClient;
#ifdef HTTP_SERVER
EthernetServer http_server(80);
#endif
PubSubClient mqtt(ethClient);
Servo servo;
dht DHT;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS_sensors(&oneWire);
//DeviceAddress addr_T_radiator[] = { 0x28, 0xFF, 0xFD, 0x6D, 0x53, 0x15, 0x01, 0xDD }; ///////////зпменить
//DeviceAddress addr_T_in[] = { 0x28, 0x9C, 0xE2, 0x44, 0x06, 0x00, 0x00, 0xB4 };

modbus_t telegram[TELEGRAMSIZE];
unsigned long u32wait;
uint8_t u8state; //!< состояние машины
uint8_t u8query; //!< указатель на запрос сообщения
uint16_t data[3][16]; //!< массив данных для совместного использования сети Modbus

unsigned long prevMillis   = 0; // для reconnect
unsigned long prevMillis2  = 0; // для публикации топиков
char buffer[100];

byte ServoAngle = 100; //Угол сервопривода при включении
bool Window = false;
bool old_Window = true;
bool Motion = false;
bool old_Motion = true;
float Hout = 0; //DHT22 Влажность
float Tout = 0; //DHT22 Температура

void setup() {
  MCUSR = 0;
  wdt_disable();
  DS_sensors.begin();
  pinMode (2, INPUT);
  pinMode (4, INPUT);
  pinMode (5, INPUT);
  digitalWrite(4, HIGH); //Объемник
  digitalWrite(5, HIGH); //Окно
  pinMode (6, OUTPUT);
  pinMode (7, OUTPUT);

  for (int i = 0; i < 3; i++) {
    telegram[i].u8id = i + 1; // slave address
    telegram[i].u8fct = 3; // function code (this one is registers read)
    telegram[i].u16RegAdd = 0; // start address in slave
    telegram[i].u16CoilsNo = 14; // number of elements (coils or registers) to read
    telegram[i].au16reg = data[i]; // pointer to a memory array in the Arduino
  }

  for (int i = 3; i < TELEGRAMSIZE; i++) {
    telegram[i].u8id = 0;
    telegram[i].u8fct = 6;
    telegram[i].u16RegAdd = 0;
    telegram[i].u16CoilsNo = 1;
    telegram[i].au16reg = 0;
  }

  master.begin(BAUD); // baud-rate
  master.setTimeOut(2000); // Таймаут ожидания ответа от подчиненного
  u32wait = millis() + 2000;
  u8state = u8query = 0;
  EthernetSetup();
  mqttSetup();
  wdt_enable(WDTO_8S);
}

void loop() {
  wdt_reset();
#ifdef HTTP_SERVER
  checkHttp();
#endif
  checkmqtt();
  getSensors();
  shiftqueue();
  SenCmd();

  if (millis() - prevMillis2 > 5000) {
    prevMillis2 = millis();
    PubTopic();
  }

  switch ( u8state ) {
    case 0:
      if (millis() > u32wait) u8state++; // wait state
      break;
    case 1:
      master.query( telegram[u8query] ); // send query (only once)
      u8state++;
      u8query++;
      if (u8query > NUMWINDOW - 1) {
        u8query = 0;
      }
      break;
    case 2:
      master.poll(); // check incoming messages
      if (master.getState() == COM_IDLE) {
        u8state = 0;
        u32wait = millis() + 2000;
      }
      break;
  }
}

