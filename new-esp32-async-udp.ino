#include "lib.h"
#include <WiFi.h>
#define _GLIBCXX_USE_CXX11_ABI 0

#define RELAY_1 26
#define RELAY_2 25
#define RELAY_3 33
#define RELAY_4 32

#define SDA_1 5
#define SCL_1 18

#define SDA_2 19
#define SCL_2 21

Adafruit_INA219 ina219_A1;
Adafruit_INA219 ina219_A2(0x41);
Adafruit_INA219 ina219_A3(0x44);
Adafruit_INA219 ina219_B1;
Adafruit_INA219 ina219_B2(0x45);

bool isConnectToServer = false;
WiFiClient client;

WorkServer serv;
bool isAuthToServer = false;
bool isAuthFromServer = false;

// Структуры данных для подключенных источников и потребилетелей
Ina ina;
InputAndOutput pzemClass;
InputAndOutput groupInputSolar;
InputAndOutput groupInputWind;
Input input;
Battery battery;
Group groupOut1;
Group groupOut2;
Group groupOut3;
WorkWithServer infoToServer;

// Объект модуля с четырьмя реле
Relay relay(RELAY_1, RELAY_2, RELAY_3, RELAY_4);

// Объекты для работы с модулем измерения переменного тока и напряжения
HardwareSerial PzemSerial2(2);     // Use hwserial UART2 at pins IO-16 (RX2) and IO-17 (TX2)
PZEM004T pzem(&PzemSerial2);
IPAddress pzemIP(192, 168, 1, 1);

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT); // настройства светодиода для индикации наличия авторизации с сервером
  delay(100);

  //Настроим внутренее АЦП на 12bit
  //analogReadResolutoin (12);

  // Установка адреса модуля PZEM004T
  pzem.setAddress(pzemIP);

  //Инициализация wifi
  WifiInit();
}

void loop() {
//
  while (!isConnectToServer) {
    //connect to server
    const uint16_t port = 2014; // port TCP server
    const char * host = "192.168.88.212"; // ip or dns
    Serial.print("Connecting to ");
    Serial.println(host);
      // Use WiFiClient class to create TCP connections
    if (!client.connect(host, port)) {
        Serial.println("Connection failed.");
        Serial.println("Waiting 5 seconds before retrying...");
        delay(1000);
    } else {
      isConnectToServer = true;
    }
    //client.print("hello server im from esp");
  }

  if (!isAuthToServer) {
    serv.authToServer(client);
    Serial.println("authToServer");
    isAuthToServer = true;
    delay (3000);
  }

  if (!isAuthFromServer) {
    serv.authFromServer(client);
    Serial.println("authFromServer");
    isAuthFromServer = true;
  }

  
  
  // Получение статусов модуля реле
  groupOut1.status = (digitalRead(RELAY_1) == LOW) ? 1 : 0;
  groupOut2.status = (digitalRead(RELAY_2) == LOW) ? 1 : 0;
  groupOut3.status = (digitalRead(RELAY_3) == LOW) ? 1 : 0;

  //battery.setV(analogRead(14)/4096*3.3/5);

    TwoWire I2Cone = TwoWire(0);
    TwoWire I2Ctwo = TwoWire(1);

    I2Cone.begin(SDA_1, SCL_1);
    I2Ctwo.begin(SDA_2, SCL_2);

    ina219_A1.begin (&I2Cone);
    ina219_A2.begin (&I2Cone);
    ina219_A3.begin (&I2Cone);
    ina219_B1.begin (&I2Ctwo);
    ina219_B2.begin (&I2Ctwo);

    float shuntvoltageA1 = ina219_A1.getBusVoltage_V();
    float shuntvoltageA2 = ina219_A2.getBusVoltage_V();
    float shuntvoltageA3 = ina219_A3.getBusVoltage_V();
    float shuntvoltageB1 = ina219_B1.getBusVoltage_V();
    float shuntvoltageB2 = ina219_B2.getBusVoltage_V();

    groupOut1.setV(shuntvoltageA1);
    groupOut2.setV(shuntvoltageA2);
    groupOut3.setV(shuntvoltageA3);

//  ina.getCurrent(ina219_A1, ina219_A2, ina219_A3, ina219_B1, ina219_B2);
//  groupOut1.setA(ina.currentA1);
//  groupOut2.setA(ina.currentA2);
//  groupOut3.setA(ina.currentA3);
//
//  client.print(groupOut1.getA());

  infoToServer.pzemTimer = millis();

    pzemClass.setV(pzem.voltage(pzemIP));
    if (pzemClass.getV() >= 0.0) {
      input.setAc_v(pzemClass.getV());
    }

    pzemClass.setA(pzem.current(pzemIP));
    if(pzemClass.getA() >= 0.0) { 
      input.setAc_a(pzemClass.getA());
    }

//    while (client.available()){
//      char c = client.read();
//      Serial.print(c);
//    }

//    Serial.println(groupOut1.getV());
//    Serial.println(groupOut2.getV());
//    Serial.println(groupOut3.getV());
//    Serial.println(groupOut1.getA());
//    Serial.println(groupOut2.getA());
//    Serial.println(groupOut3.getA());
//    Serial.println();

    //serv.requestSendData(client, groupOut1, groupOut2, groupOut3, groupInputSolar, groupInputWind, pzemClass, battery);
  //groupInputSolar.getV();

  int countForward = 0;
  while (client.available()) {
    char c = client.read();
    Serial.print("     ");
    Serial.print(countForward);
    Serial.println(c);
    
    if ((countForward == 8) && (c == '3')) {
      const char* sendDataToServer = "{\"type\": 3, \"data\": {\"solar\": [%4.2f, %4.2f, %d], \"wind\": [%4.2f, %4.2f, %d], \"gen\": [%4.2f, %4.2f, %d], \"bat\": [%4.2f], \"1\": [%4.2f, %4.2f, %d], \"2\": [%4.2f, %4.2f, %d], \"3\": [%4.2f, %4.2f, %d]}}\n";
      char AlreadyDataToServer[1000];
      sprintf(AlreadyDataToServer, sendDataToServer, groupInputSolar.getV(), groupInputSolar.getA(), groupInputSolar.status, groupInputWind.getV(), groupInputWind.getA(), groupInputWind.status, pzemClass.getV(), pzemClass.getA(), pzemClass.status, battery.getV(), groupOut1.getV(), groupOut1.getA(), groupOut1.status, groupOut2.getV(), groupOut2.getA(), groupOut2.status, groupOut3.getV(), groupOut3.getA(), groupOut3.status);
      Serial.println(AlreadyDataToServer);
  //       const char* sendDataToServer = "{\"type\": 3, \"data\": {\"solar\": [%4.2f, %4.2f, %d], \"wind\": [%4.2f, %4.2f, %d], \"gen\": [%4.2f, %4.2f, %d], \"bat\": [%4.2f], \"1\": [%4.2f, %4.2f, %d], \"2\": [%4.2f, %4.2f, %d], \"3\": [%4.2f, %4.2f, %d]}}\n", 
  //  &groupInputSolar.getV(), &groupInputSolar.getA(), groupInputSolar.status, groupInputWind.getV(), groupInputWind.getA(), groupInputWind.status, pzemClass.getV(), pzemClass.getA(), pzemClass.status, battery.getV(), groupOut1.getV(), groupOut1.getA(), groupOut1.status, groupOut2.getV(), groupOut2.getA(), groupOut2.status, groupOut3.getV(), groupOut3.getA(), groupOut3.status 
  //  ;
      client.print(AlreadyDataToServer);
      //break;
    } else if ((countForward == 8) && (c == '4')) {
      //{"type": 4,"data": {"rn": 1,"status": 1}}
      //25 - 1 .  37 - status
      //Serial.println("kek");
      //break;
    }
    countForward++;
  }

    delay (1000);
    
}
