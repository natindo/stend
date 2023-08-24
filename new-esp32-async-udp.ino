#include "lib.h"
#include <WiFi.h>
#include <cstring>
#include <thread>
<<<<<<< HEAD
=======
#include "esp_structs.h"
#include "newserver.h"
#include <utility>
<<<<<<< HEAD
>>>>>>> 860479c (check 1)
#define _GLIBCXX_USE_CXX11_ABI 0
=======
// #define _GLIBCXX_USE_CXX11_ABI 0
>>>>>>> 2f7f745 (add work with server and complite reley switch)

#define RELAY_1 26
#define RELAY_2 25
#define RELAY_3 33
#define RELAY_4 32

#define SDA_1 5
#define SCL_1 18

#define SDA_2 19
#define SCL_2 21

#define MSG_BUFF_SIZE 150

<<<<<<< HEAD
=======
//объекты измерителей тока и напряжения
>>>>>>> 860479c (check 1)
Adafruit_INA219 ina219_A1;
Adafruit_INA219 ina219_A2(0x41);
Adafruit_INA219 ina219_A3(0x44);
Adafruit_INA219 ina219_B1;
Adafruit_INA219 ina219_B2(0x45);

bool isConnectToServer = false;
WiFiClient client;

bool isAuthToServer = false;
bool isAuthFromServer = false;

// Структуры данных для подключенных источников и потребилетелей
InputAndOutput pzemClass;
InputAndOutput groupInputSolar;
InputAndOutput groupInputWind;
Input input;
Battery battery;
Group groupOut1;
Group groupOut2;
Group groupOut3;
<<<<<<< HEAD
WorkWithServer infoToServer;

<<<<<<< HEAD
=======
=======
>>>>>>> 2f7f745 (add work with server and complite reley switch)

RelayData solar;
RelayData wind;
RelayData generator;
RelayData consumers[RELAYS_NUMBER];
int batteryVoltage;


>>>>>>> 860479c (check 1)
// Объект модуля с четырьмя реле
Relay relay(RELAY_1, RELAY_2, RELAY_3, RELAY_4);

// Объекты для работы с модулем измерения переменного тока и напряжения
HardwareSerial PzemSerial2(2);     // Use hwserial UART2 at pins IO-16 (RX2) and IO-17 (TX2)
PZEM004T pzem(&PzemSerial2);
IPAddress pzemIP(192, 168, 1, 1);

<<<<<<< HEAD
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

=======
int needPower(){

    groupOut1.setV(ina219_A1.getBusVoltage_V());
    groupOut2.setV(ina219_A2.getBusVoltage_V());
    groupOut3.setV(ina219_A3.getBusVoltage_V());
    
    groupOut1.setA(ina219_A1.getCurrent_mA());
    groupOut2.setA(ina219_A2.getCurrent_mA());
    groupOut3.setA(ina219_A3.getCurrent_mA());

    groupInputSolar.setV(ina219_B1.getBusVoltage_V());
    groupInputWind.setV(ina219_B2.getBusVoltage_V());
    groupInputSolar.setA(ina219_B1.getCurrent_mA());
    groupInputWind.setA(ina219_B2.getCurrent_mA());


    return ( groupInputSolar.getV()*groupInputSolar.getA()+groupInputWind.getV()*groupInputWind.getA()<
    ina219_A1.getBusVoltage_V()*ina219_A1.getCurrent_mA()+ina219_A2.getBusVoltage_V()*ina219_A2.getCurrent_mA()+ina219_A3.getBusVoltage_V()*ina219_A3.getCurrent_mA()) 
    ?1:0;
}

int low50(){
    return (12.4 - (analogRead(35) * 3.3 / 4096 * 5) / (12.4 - 7.2) < 0.5) ? 1 : 0;
}

void volk() {

    if (needPower()) {
        relay.turnON(4);
    }
    else {
        relay.turnOFF(4);
    }


    if (ina219_A1.getCurrent_mA() + ina219_A2.getCurrent_mA() + ina219_A3.getCurrent_mA() > 1800) {
        relay.turnOFF(3);
    } else if (ina219_A1.getCurrent_mA() + ina219_A2.getCurrent_mA() > 1800) {
        relay.turnOFF(2);
    }


    if (ina219_A1.getCurrent_mA() + ina219_A2.getCurrent_mA() < 1000) {
        relay.turnON(3);
    } else if (ina219_A1.getCurrent_mA() < 1000) {
        relay.turnON(2);
    }
}

void ecoWork () {

    relay.turnON(1);
    int prog = 1;/*Прогноз генерации больше прогноза потребления? ----  1*//*Прогноз генерации больше прогноза потребления на 1/2? ------ 2*/
    if (prog == 1) {
        if (prog == 2) {
            if (needPower()) {
                if (digitalRead(RELAY_3) == LOW) {
                    relay.turnOFF(3);
                }
                if (needPower()) {
                    if (digitalRead(RELAY_2) == LOW) {
                        if ((ina219_A2.getCurrent_mA()>800)) {
                            relay.turnOFF(2);
                        } else {
                            if (digitalRead(RELAY_4) == LOW) {
                                relay.turnOFF(4);
                            }
                        }
                        if (needPower()) {
                            if (digitalRead(RELAY_4) == HIGH) {
                                relay.turnON(4);
                            }
                        } else { // не требуется доп мощность
                            if (digitalRead(RELAY_4) == LOW) {
                                relay.turnOFF(4);
                            }
                        }
                    } else {
                        if (digitalRead(RELAY_4) == LOW) {
                            relay.turnOFF(4);
                        }
                    }
                } else {
                    if (digitalRead(RELAY_4) == LOW) {
                        relay.turnOFF(4);
                    }
                }
            } else {
                if (digitalRead(RELAY_4) == LOW) {
                    relay.turnOFF(4);
                } else {
                    if (digitalRead(RELAY_2) == LOW) {
                        if (digitalRead(RELAY_3) == LOW) {
                        } else {
                            relay.turnON(3);
                            if (needPower()) {
                                relay.turnOFF(2);
                            }
                        }
                    } else {
                        relay.turnON(2);
                        if (needPower()) {
                            relay.turnOFF(2);
                        }
                    }
                }
            }
        } else {
            if (low50()) {
                if (digitalRead(RELAY_3) == LOW) {
                    relay.turnOFF(3);
                }
                if (needPower()) {
                    if (digitalRead(RELAY_2) == LOW) {
                        relay.turnOFF(2);
                    }
                    if (needPower()) {
                        relay.turnON(4);
                    }
                }
            } else {
                if (digitalRead(RELAY_2) == HIGH && !needPower()) {
                    relay.turnON(2);
                }
                if (needPower()) {
                    relay.turnON(4);
                    relay.turnOFF(2);
                }
            }
        }
    } else {
        if (digitalRead(RELAY_3) == LOW) {
            relay.turnOFF(3);
        }
        if (digitalRead(RELAY_2) == LOW) {
            relay.turnOFF(2);
        }
        if (needPower() && digitalRead(RELAY_4) == HIGH) {
            relay.turnON(4);
        }
    }
}


void economicalWork () {
    relay.turnON(1);
    if (!low50()) {
        if (needPower()) {
            if (digitalRead(RELAY_3) == LOW) {
                relay.turnOFF(3);
                if (!needPower()) {
                    relay.turnOFF(4);
                }
            }
            if ((digitalRead(RELAY_2) == LOW) && (ina219_A2.getCurrent_mA() > 800)) {/*включена ли вторая группа и ее мощность больше 50%*/
                relay.turnOFF(2);
                if (!needPower()) {
                    relay.turnOFF(4);
                } else if (needPower()){
                    relay.turnON(4);
                }
            }

        } else if (!needPower()) {
            if ((digitalRead(RELAY_4) == LOW)) {
                relay.turnOFF(4);
            } else { //генератор не влючён
                if (digitalRead(RELAY_2) == LOW) {
                    if (digitalRead(RELAY_3) == LOW) {
                        //заряжать акб --- значит нечего не делать
                    } else { //3 группа выключена
                        relay.turnON(3);
                        if (needPower()) {
                            relay.turnOFF(2);
                        }
                    }
                } else {
                    relay.turnON(2);
                    if (needPower()) {
                    relay.turnOFF(2);
                    }
                }
            }
        }
    } else if (low50()) {
        if ((digitalRead(RELAY_3) == LOW)) {
            relay.turnOFF(3);
        }
        if ((digitalRead(RELAY_2) == LOW)) {
            relay.turnOFF(2);
        }
        if (needPower()) {
        //
            if ((digitalRead(RELAY_4) == HIGH)) {
                relay.turnON(4);
            }
        } else {
            if ((digitalRead(RELAY_4) == LOW)) {
                relay.turnON(4);
            }
        }
    }
}

// void xvataetLiKZadannomuChasy (float raznicaToka, int tekuchayZarydkaAKB) {
//     bool groupOut1Status = (digitalRead(RELAY_1) == LOW) ? 1 : 0;
//     bool groupOut2Status = (digitalRead(RELAY_2) == LOW) ? 1 : 0;
//     bool groupOut3Status = (digitalRead(RELAY_3) == LOW) ? 1 : 0;

//     relay.turnOFF(1);
//     relay.turnOFF(2);
//     relay.turnOFF(3);

//     int batteryVoltage = analogRead(35)*3.3/4096*5;

    
// }

int powerConsumer3 = 0;
int powerConsumer2 = 0;

double percentageOfCharge (Group groupOut1, Group groupOut2, Group groupOut3, InputAndOutput groupInputSolar, InputAndOutput groupInputWind, InputAndOutput pzemClass) {
    return floor((battery.getV()+(0.00026*(groupOut1.getA()+
                                           groupOut2.getA()+
                                           groupOut3.getA()-
                                           groupInputSolar.getA()-
                                           groupInputWind.getA()-
                                           pzemClass.getA()))-
                                           7.2)/(12.6-7.2)*100+0.5);
}

double timeUntilBatteryIsCharged (Group groupOut1, Group groupOut2, Group groupOut3, InputAndOutput groupInputSolar, InputAndOutput groupInputWind, InputAndOutput pzemClass) {
    return (2200.0*2*(0.9-percentageOfCharge(groupOut1, groupOut2, groupOut3, groupInputSolar, groupInputWind, pzemClass)/100)/(
                                                   groupOut1.getA()+
                                                   groupOut2.getA()+
                                                   groupOut3.getA()-
                                                   groupInputSolar.getA()-
                                                   groupInputWind.getA()-
                                                   pzemClass.getA()))*0.7;
}

double timeUntilBatteryIsCharged (Group groupOut1, int powerConsumer2, Group groupOut3, InputAndOutput groupInputSolar, InputAndOutput groupInputWind, InputAndOutput pzemClass) {
    return (2200.0*2*(0.9-percentageOfCharge(groupOut1, groupOut2, groupOut3, groupInputSolar, groupInputWind, pzemClass)/100)/(
                                                   groupOut1.getA()+
                                                   powerConsumer2+
                                                   groupOut3.getA()-
                                                   groupInputSolar.getA()-
                                                   groupInputWind.getA()-
                                                   pzemClass.getA()))*0.7;
}

double timeUntilBatteryIsCharged (Group groupOut1, Group groupOut2, int powerConsumer3, InputAndOutput groupInputSolar, InputAndOutput groupInputWind, InputAndOutput pzemClass) {
    return (2200.0*2*(0.9-percentageOfCharge(groupOut1, groupOut2, groupOut3, groupInputSolar, groupInputWind, pzemClass)/100)/(
                                                   groupOut1.getA()+
                                                   groupOut2.getA()+
                                                   powerConsumer3-
                                                   groupInputSolar.getA()-
                                                   groupInputWind.getA()-
                                                   pzemClass.getA()))*0.7;
}

double timeUntilBatteryIsCharged (Group groupOut1, Group groupOut2, Group groupOut3, InputAndOutput groupInputSolar, InputAndOutput groupInputWind) {
    return (2200.0*2*(0.9-percentageOfCharge(groupOut1, groupOut2, groupOut3, groupInputSolar, groupInputWind, pzemClass)/100)/(
                                                   groupOut1.getA()+
                                                   groupOut2.getA()+
                                                   groupOut3.getA()-
                                                   groupInputSolar.getA()-
                                                   groupInputWind.getA()))*0.7;
}

void MaximumPowerWork (RelayData* consumers, double timeToChargeFromServer) {
    //приходит сколько может быть 
    int time = millis();
    if (timeToChargeFromServer > timeUntilBatteryIsCharged(groupOut1, groupOut2, groupOut3, groupInputSolar, groupInputWind, pzemClass)) {
        if (digitalRead(RELAY_2) == HIGH) {
            if (timeToChargeFromServer > timeUntilBatteryIsCharged(groupOut1, powerConsumer2, groupOut3, groupInputSolar, groupInputWind, pzemClass)/*акб зарядится с учётом потребления*/) {
                relay.turnON(2);
            }
        }
        if (digitalRead(RELAY_3) == HIGH) {
            if (timeToChargeFromServer > timeUntilBatteryIsCharged(groupOut1, groupOut2, powerConsumer3, groupInputSolar, groupInputWind, pzemClass)/*акб зарядится с учётом потребления*/) {
                relay.turnON(3);
            }
        }
        if (digitalRead(RELAY_4) == LOW) {
            if (timeToChargeFromServer > timeUntilBatteryIsCharged(groupOut1, groupOut2, groupOut3, groupInputSolar, groupInputWind)/*акб зарядиться к нужному часу без дизеля*/) {
                relay.turnOFF(4);
            }
        }   
    } else {
        if (digitalRead(RELAY_4) == HIGH) {
            relay.turnON(4);
        }
        if (timeToChargeFromServer < timeUntilBatteryIsCharged (groupOut1, groupOut2, groupOut3, groupInputSolar, groupInputWind, pzemClass)) {
            if (digitalRead(RELAY_3) == LOW) {
                powerConsumer3 = consumers[2].voltage * consumers[2].current;
                relay.turnOFF(3);
            }
        }
        if (timeToChargeFromServer < timeUntilBatteryIsCharged (groupOut1, groupOut2, groupOut3, groupInputSolar, groupInputWind, pzemClass)) {
            if (digitalRead(RELAY_2) == LOW) {
                powerConsumer2 = consumers[1].voltage * consumers[1].current;
                relay.turnOFF(2);
            }
        } else {
            //вывод о невозможности
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT); // настройства светодиода для индикации наличия авторизации с сервером
    delay(100);

    // Установка адреса модуля PZEM004T
    pzem.setAddress(pzemIP);

    //Инициализация wifi
    WifiInit();
}

bool loginSuc = false;
uint16_t ms = 0;

void loop() {
    //попытка законнектиться к серверу
    while (!isConnectToServer) {
        //connect to server
        const uint16_t port = 2000; // port TCP server
        const char * host = "192.168.3.10"; // ip or dns
        Serial.print("Connecting to ");
        Serial.println(host);
        // Use WiFiClient class to create TCP connections
        if (!client.connect(host, port)) {
            Serial.println("Connection failed.");
            Serial.println("Waiting 2 seconds before retrying...");
            delay(2000);
        } else {
            isConnectToServer = true;
        }
    }

    //решить с Эдиком

    // if (!loginSuc) {
    //     char* login = "priority2030";
    //     char* pass = "12345";
    //     auto [auth, counter] = encodeAuth(login, pass);
    //     for (int i = 0; i < counter; i++) {
    //         client.print(*auth);
    //         //Serial.println(*auth);
    //         auth++;
    //     }
    //     loginSuc = true;
    //     Serial.println("login succsess");
    // }

    loginSuc = true;


    // char* login = "priority2030";
    // char* pass = "12345";
    // auto [auth, counter] = encodeAuth(login, pass);
    // for (int i = 0; i < counter; i++) {
    //     client.print(*auth);
    //     //Serial.println(*auth);
    //     auth++;
    // }

    //client.print(auth);
    // Получение статусов модуля реле
    groupOut1.status = (digitalRead(RELAY_1) == LOW) ? 1 : 0;
    groupOut2.status = (digitalRead(RELAY_2) == LOW) ? 1 : 0;
    groupOut3.status = (digitalRead(RELAY_3) == LOW) ? 1 : 0;

    //инициализация I2C и INA
>>>>>>> 860479c (check 1)
    TwoWire I2Cone = TwoWire(0);
    TwoWire I2Ctwo = TwoWire(1);

    I2Cone.begin(SDA_1, SCL_1);
    I2Ctwo.begin(SDA_2, SCL_2);

    ina219_A1.begin (&I2Cone);
    ina219_A2.begin (&I2Cone);
    ina219_A3.begin (&I2Cone);
    ina219_B1.begin (&I2Ctwo);
    ina219_B2.begin (&I2Ctwo);

<<<<<<< HEAD
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

=======
    ina219_A1.powerSave(0);
    ina219_A2.powerSave(0);
    ina219_A3.powerSave(0);
    ina219_B1.powerSave(0);
    ina219_B2.powerSave(0);

    //буффер для обработки запросов с сервера
    static char buffer[MSG_BUFF_SIZE];
    int workMode = 0; //режим работы (ручной или любой другой)

    //Serial.println("ASDFGFSDG");
    if (loginSuc) {
    //пока сервер подключен
        while (client.connected()) {

            //Serial.println("check1");

            // Serial.println(millis());
            // Serial.println(ms);

            if (millis() - ms >= 10000) {

                Serial.println("CheckSensorsDataSent");

                pzemClass.setV(pzem.voltage(pzemIP));
                if (pzemClass.getV() >= 0.0) {
                    input.setAc_v(pzemClass.getV());
                }

                pzemClass.setA(pzem.current(pzemIP));
                if(pzemClass.getA() >= 0.0) { 
                    input.setAc_a(pzemClass.getA() / 1000);
                }

                // Serial.println("checksend1");

                solar.current = ina219_B1.getCurrent_mA();
                solar.voltage = ina219_B1.getBusVoltage_V();
                wind.current = ina219_B2.getCurrent_mA();
                wind.voltage = ina219_B2.getBusVoltage_V();
                generator.current = input.getAc_a();
                generator.voltage = input.getAc_v();
                batteryVoltage = floor(analogRead(35)*3.3/4096*5*10+0.5)/105;

                // Serial.println("checksend2");

                consumers[0].voltage = ina219_A1.getBusVoltage_V();
                consumers[1].voltage = ina219_A2.getBusVoltage_V();
                consumers[2].voltage = ina219_A3.getBusVoltage_V();
                consumers[0].current = ina219_A1.getCurrent_mA();
                consumers[1].current = ina219_A2.getCurrent_mA();
                consumers[2].current = ina219_A3.getCurrent_mA();

                // Serial.println("checksend3");

                //решить с Эдиком
                //char AlreadyDataToServer[200];
                // auto [AlreadyDataToServer, counter_data] = 
                // char* AlreadyDataToServer = encodeSensorsData(solar, wind, generator, batteryVoltage, consumers).first;
                // int counterData = encodeSensorsData(solar, wind, generator, batteryVoltage, consumers).second;

                // Serial.println("checksend2");

                Serial.println(analogRead(35));

                std::pair<char*, int> result = encodeSensorsData(solar, wind, generator, batteryVoltage, consumers);
                char* AlreadyDataToServer = result.first;
                int counterData = result.second;

                // char* AlreadyDataToServer;
                // int counterData;

                //encodeSensorsData(solar, wind, generator, batteryVoltage, consumers, AlreadyDataToServer, counterData);

                // Serial.println("checksend4");
                for (int i = 0; i < counterData; i++) {
                    client.print(*AlreadyDataToServer++);
                } 
                // Serial.println("checksend5");
                ms = millis();

            }

            //Serial.println(123);
            int size = 0;
            size = client.available();
            //Serial.println(size);
            int size_check = size;
            int i = 0;
            //int workMode = 0; //режим работы (ручной или любой другой)

            while (size) {
                Serial.println("check2");
                buffer[i] = client.read();
                //Serial.println(buffer[i]);
                --size;
                ++i;
            }

            Datagram* d = reinterpret_cast<Datagram*>(buffer);
            if (size_check != 0) {
                //Serial.println(size_check);
                switch (d->type) {
                case MODE: {
                    Mode* modeData = reinterpret_cast<Mode*>(d + 1);

                    Serial.println("switchModeCheck");

                    switch (modeData->mode) {
                    case Manual: {
                        workMode = 0;
                        Serial.println("check21");
                        break;
                    }
                    case MaximumReliability: {
                        workMode = 1;
                        Serial.println("check31");
                        break;
                    }
                    case MaximumPower: {
                        workMode = 2;
                        Serial.println("check41");
                        break;
                    }
                    case MaximumEcology: {
                        workMode = 3;
                        Serial.println("check51");
                        break;
                    }
                    case MaximumEconomy:{
                        workMode = 4;
                        Serial.println("check61");
                        break;
                    }
                    }
                    break;

                    // modeData->mode - код нового выбранного режима
                    // далее работает выбранный алгоритм
                }
                case SWITCH_RELAY: {
                    SwitchRelay* relayData = reinterpret_cast<SwitchRelay*>(d + 1);
                    // relayData->relayNumber - номер переключенного реле
                    // relayData->status - новый статус реле
                    // далее в коде переключить реле

                    if (relayData->status == 0) {
                        relay.turnOFF (relayData->relayNumber);
                    } else if (relayData->status == 1) {
                        relay.turnON (relayData->relayNumber);
                    }
                    Serial.println(relayData->status);
                    Serial.println(relayData->relayNumber);
                    break;
                }
                case SENSORS_DATA: {
                    pzemClass.setV(pzem.voltage(pzemIP));
                    if (pzemClass.getV() >= 0.0) {
                        input.setAc_v(pzemClass.getV());
                    }

                    pzemClass.setA(pzem.current(pzemIP));
                    if(pzemClass.getA() >= 0.0) { 
                        input.setAc_a(pzemClass.getA() / 1000);
                    }

                    solar.current = ina219_B1.getCurrent_mA();
                    solar.voltage = ina219_B1.getBusVoltage_V();
                    wind.current = ina219_B2.getCurrent_mA();
                    wind.voltage = ina219_B2.getBusVoltage_V();
                    generator.current = input.getAc_a();
                    generator.voltage = input.getAc_v();
                    // batteryVoltage = analogRead(35)*3.3/4096*5;
                    batteryVoltage = (floor(analogRead(35)*3.3/4096*5*10+0.5)/10);

                    consumers[0].voltage = ina219_A1.getBusVoltage_V();
                    consumers[1].voltage = ina219_A2.getBusVoltage_V();
                    consumers[2].voltage = ina219_A3.getBusVoltage_V();
                    consumers[0].current = ina219_A1.getCurrent_mA();
                    consumers[1].current = ina219_A2.getCurrent_mA();
                    consumers[2].current = ina219_A3.getCurrent_mA();

                    //решить с Эдиком
                    // char* AlreadyDataToServer;
                    // auto [AlreadyDataToServer, counterData] = encodeSensorsData(solar, wind, generator, batteryVoltage, consumers);
                    char* AlreadyDataToServer = encodeSensorsData(solar, wind, generator, batteryVoltage, consumers).first;
                    int counterData = encodeSensorsData(solar, wind, generator, batteryVoltage, consumers).second;
                    for (int i = 0; i < counterData; i++) {
                        client.print(AlreadyDataToServer++);
                    }   

                    break;
                    // client.print(AlreadyDataToServer);
                    // Serial.println(AlreadyDataToServer);
                }
                case SUCCESS: {
                    Serial.println ("Авторизация успешна ");
                    break;
                }
                case ERROR: {
                    Serial.println ("Ошибка авторизации");
                    exit(EXIT_FAILURE);
                    break;
                }
                }
            }

            switch (workMode) {
            case 0: {
                //skip
                //Serial.println("check11");
                break;
            }
            case 1: {
                //MaximumReliability();
                Serial.println("check12");
                break;
            }
            case 2: {
                //MaximumPower
                Serial.println("check13");
                //MaximumPowerWork();
                break;
            }
            case 3: {
                //MaximumEcology
                Serial.println("check14");
                //ecoWork();
                break;
            }
            case 4: {
                //MaximumEconomy
                Serial.println("check15");
                //economicalWork();
                break;
            }
            }

            for (int i = 0; i < MSG_BUFF_SIZE; ++i) {
                buffer[i] = 9;
            }

        }
    }

    //если вышли из предыдущего вайла - коннект потерян
    isConnectToServer = false;
    isAuthToServer = false;
    isAuthFromServer = false;

    Serial.println("");
>>>>>>> 860479c (check 1)
    delay (200);
    
}
