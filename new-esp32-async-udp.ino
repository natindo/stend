#include "lib.h"
#include <WiFi.h>
#include <cstring>
#include <thread>
#define _GLIBCXX_USE_CXX11_ABI 0

#define RELAY_1 26
#define RELAY_2 25
#define RELAY_3 33
#define RELAY_4 32

#define SDA_1 5
#define SCL_1 18

#define SDA_2 19
#define SCL_2 21

#define MSG_BUFF_SIZE 150

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

struct Data
{
  
};

void loop() {
//
  //Ina data_ina;
  //Adafruit_INA219 &ina219_A1, Adafruit_INA219 &ina219_A2, Adafruit_INA219 &ina219_A3, Adafruit_INA219 &ina219_B1, Adafruit_INA219 &ina219_B2
  
  //std::thread th(&Ina::getVoltage, std::ref(Ina), std::ref(ina219_A1), std::ref(ina219_A2), std::ref(ina219_A3), std::ref(ina219_B1), std::ref(ina219_B2));
  
  while (!isConnectToServer) {
    //connect to server
    const uint16_t port = 2001; // port TCP server
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
    delay (500);
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

//    float shuntvoltageA1 = ina219_A1.getShuntVoltage_mV();
//    float shuntvoltageA2 = ina219_A2.getShuntVoltage_mV();
//    float shuntvoltageA3 = ina219_A3.getShuntVoltage_mV();
//    float shuntvoltageB1 = ina219_B1.getBusVoltage_V();
//    float shuntvoltageB2 = ina219_B2.getBusVoltage_V();
//
//    groupOut1.setV(shuntvoltageA1);
//    groupOut2.setV(shuntvoltageA2);
//    groupOut3.setV(shuntvoltageA3);
//
//  infoToServer.pzemTimer = millis();
//
//    pzemClass.setV(pzem.voltage(pzemIP));
//    if (pzemClass.getV() >= 0.0) {
//      input.setAc_v(pzemClass.getV());
//    }
//
//    pzemClass.setA(pzem.current(pzemIP));
//    if(pzemClass.getA() >= 0.0) { 
//      input.setAc_a(pzemClass.getA());
//    }

    static char buffer[MSG_BUFF_SIZE] = { 0 };
    
    while (client.connected()) { 




//    Serial.println(shuntvoltageA1);
//    Serial.println(shuntvoltageA2);
//    Serial.println(shuntvoltageA2);
    
//    float shuntvoltageB1 = ina219_B1.getBusVoltage_V();
//    float shuntvoltageB2 = ina219_B2.getBusVoltage_V();
//
//    groupOut1.setV(shuntvoltageA1);
//    groupOut2.setV(shuntvoltageA2);
//    groupOut3.setV(shuntvoltageA3);

  infoToServer.pzemTimer = millis();

    pzemClass.setV(pzem.voltage(pzemIP));
    if (pzemClass.getV() >= 0.0) {
      input.setAc_v(pzemClass.getV());
    }

    pzemClass.setA(pzem.current(pzemIP));
    if(pzemClass.getA() >= 0.0) { 
      input.setAc_a(pzemClass.getA());
    }


      
      bool isSendData = false;
      bool isRelaySwitch = false;
    uint size = client.available();

    if (size) {
      uint openings = 0;
      uint closures = 0;

      char input = client.read();

      while (input != '{' && size--)  // read and skip all shit from socket
      input = client.read();

      if (size) {
        ++openings;
        buffer[0] = input;
      }
      else {
        // fuck
      }

      uint i = 1;

      for (; openings != closures && size && i < MSG_BUFF_SIZE - 1; ++i, --size) {
        input = client.read();
        buffer[i] = input;

        openings += (input == '{');
        closures += (input == '}');
      }

      if (openings != closures && !size) {
        Serial.println("message read partly");
      }

      else if (openings != closures && i >= MSG_BUFF_SIZE - 1) {
        Serial.println("buffer size overflow");
      }

      else {  // correct message
        buffer[i] = '\0';
        Serial.println(buffer);
      }
    }
  
    if (buffer[2] == 'd' && isRelaySwitch == false) {
//      Serial.println(shuntvoltageA1);
//    Serial.println(shuntvoltageA2);
//    Serial.println(shuntvoltageA2);
      int relayNum = buffer[18] - '0';
      int relayPosition = buffer[31] - '0';
      if (relayPosition == 1) {
        relay.turnON(relayNum + 1);
      } else if (relayPosition == 0) {
        relay.turnOFF(relayNum + 1);
      }
      isRelaySwitch = true;
    } else if (buffer[2] == 't' && isSendData == false) {



    float shuntvoltageA1 = ina219_A1.getBusVoltage_V();
    float shuntvoltageA2 = ina219_A2.getBusVoltage_V();
    float shuntvoltageA3 = ina219_A3.getBusVoltage_V();

    groupOut1.setV(shuntvoltageA1);
    groupOut2.setV(shuntvoltageA2);
    groupOut3.setV(shuntvoltageA3);

    Serial.println(shuntvoltageA1);
    Serial.println(shuntvoltageA2);
    Serial.println(shuntvoltageA3);


      
        const char* sendDataToServer = "{\"type\": 3, \"data\": {\"solar\": [%4.2f, %4.2f, %d], \"wind\": [%4.2f, %4.2f, %d], \"gen\": [%4.2f, %4.2f, %d], \"bat\": [%4.2f], \"1\": [%4.2f, %4.2f, %d], \"2\": [%4.2f, %4.2f, %d], \"3\": [%4.2f, %4.2f, %d]}}\n";
        char AlreadyDataToServer[1000];
        sprintf(AlreadyDataToServer, sendDataToServer, groupInputSolar.getV(), groupInputSolar.getA(), groupInputSolar.status, groupInputWind.getV(), groupInputWind.getA(), groupInputWind.status, pzemClass.getV(), pzemClass.getA(), pzemClass.status, battery.getV(), groupOut1.getV(), groupOut1.getA(), groupOut1.status, groupOut2.getV(), groupOut2.getA(), groupOut2.status, groupOut3.getV(), groupOut3.getA(), groupOut3.status);
        client.print(AlreadyDataToServer);
        isSendData = true;
    }
    for(int i = 0; i < MSG_BUFF_SIZE; ++i) {
      buffer[i] = 0;
    }
  }





  Serial.println("");

    delay (200);
    
}
