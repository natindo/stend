#include "lib.h"
#include <WiFi.h>
#include <cstring>
#include <thread>
#include "esp_structs.h"
#include "newserver.h"
#include <utility>
// #define DEBAG

//Волк

#define RELAY_1 26
#define RELAY_2 25
#define RELAY_3 33
#define RELAY_4 32

#define SDA_1 5
#define SCL_1 18

#define SDA_2 19
#define SCL_2 21

#define MSG_BUFF_SIZE 150

//выслать данные батарейки
bool isSendBattaryVoltage = false;

//счётчик изменений статуса реле
int relaySwitchCounter = 0;

//таймер для считывания батарейки
uint32_t timerBattery = 0;

//объекты измерителей тока и напряжения
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

RelayData solar;
RelayData wind;
RelayData generator;
RelayData consumers[RELAYS_NUMBER];
uint16_t batteryVoltageToServer;
double batteryVoltage;


// Объект модуля с четырьмя реле
Relay relay(RELAY_1, RELAY_2, RELAY_3, RELAY_4);

// Объекты для работы с модулем измерения переменного тока и напряжения
HardwareSerial PzemSerial2(2);     // Use hwserial UART2 at pins IO-16 (RX2) and IO-17 (TX2)
PZEM004T pzem(&PzemSerial2);
IPAddress pzemIP(192, 168, 1, 1);

int consumption_forecast (RelayData* consumers, RelayData wind, RelayData solar, InputAndOutput pzemClass) {// выработка больше потребления по рогнозам, я просто беру производство > потребления без прогноза =1, иначе 0
//   return ((                                        consumers[0].current +
//                                                    consumers[1].current +
//                                                    consumers[2].current -
//                                                    solar.current -
//                                                    wind.current -
//                                                    pzemClass.getA() * x ) < 0 ) ? 1 : 0;

    return 1;
}

int needPower(){

    consumers[0].voltage = ina219_A1.getBusVoltage_V();
    consumers[1].voltage = ina219_A2.getBusVoltage_V();
    consumers[2].voltage = ina219_A3.getBusVoltage_V();

    consumers[0].current = ina219_A1.getCurrent_mA();
    consumers[1].current = ina219_A2.getCurrent_mA();
    consumers[2].current = ina219_A3.getCurrent_mA();

    solar.voltage = ina219_B1.getBusVoltage_V();
    solar.current = ina219_B1.getCurrent_mA();
    wind.voltage = ina219_B2.getBusVoltage_V();
    wind.current = ina219_B2.getCurrent_mA();

    return (solar.voltage * solar.current + wind.voltage * wind.current - (consumers[0].voltage * consumers[0].current +
    consumers[1].voltage * consumers[1].current + consumers[2].voltage * consumers[2].current));
}

int low50 () {
    double batteryVoltageESP = analogRead(35) * 3.3 / 4096 * 5.3; //настоящие вольты //флур мб убрать
    batteryVoltage = batteryVoltageESP + (0.00028 * (1 + 0.0001 * (batteryVoltageESP - 12.5)) * (consumers[0].current + consumers[1].current + consumers[2].current - solar.current / 2 - wind.current / 2 - pzemClass.getA() * 4)); //мнимые вольты

    // Serial.println(batteryVoltage);

    return (((batteryVoltage-7.2) / (14 - 7.2)) < 0.5) ? 1 : 0;
}

void ecoWork () { // экологичный ждёт проноза
   relay.turnON(1, consumers);
    // relay.turnOFF(4, consumers);  СДЕЛАТЬ ВЫКЛЮЧЕНИЕ РЕЛЕ ПРИ ВКЛЮЧЕНИИ!!!!!!!!!!!!!

    Serial.print("low50");
    Serial.println(low50());
    Serial.print ("consumption_forecast(consumers, wind, solar, pzemClass)");
    Serial.println(consumption_forecast(consumers, wind, solar, pzemClass));
    Serial.print("needPower()");
    Serial.println(needPower());


    if (!low50()) {
      relay.turnOFF(4, consumers);
        if (consumption_forecast(consumers, wind, solar, pzemClass) >= 1) {
          relay.turnON(2, consumers);
          if (needPower() < -7000) {
            relay.turnOFF(3, consumers);
          } else if (needPower() > 10600) {
              relay.turnON(3, consumers);
          }
        }else{
          relay.turnOFF(3, consumers);
          if (needPower() < -7000) {
            relay.turnOFF(2, consumers);
          } else if (needPower() > 10600) {
            relay.turnON(2, consumers);
          }
        }            
    } else if (low50()) {
        if (consumption_forecast(consumers, wind, solar, pzemClass) >= 1) {
            relay.turnOFF(2, consumers);
        } 
            relay.turnOFF(3, consumers);
            relay.turnON(4, consumers);
    }
}

void economicalWork () {

 relay.turnON(1, consumers);
    // relay.turnOFF(4, consumers);  СДЕЛАТЬ ВЫКЛЮЧЕНИЕ РЕЛЕ ПРИ ВКЛЮЧЕНИИ!!!!!!!!!!!!!
    Serial.print("low50");
    Serial.println(low50());
    Serial.print ("consumption_forecast(consumers, wind, solar, pzemClass)");
    Serial.println(consumption_forecast(consumers, wind, solar, pzemClass));
    Serial.print("needPower()");
    Serial.println(needPower());

    if (!low50()) {
        if (needPower() < -7000) {
            if (consumers[2].status == 1) {
                relay.turnOFF(3, consumers);
            } else { 
                relay.turnOFF(2, consumers);
            }
        } else if (needPower() > 10600) {
            if (consumers[1].status == 0) {
                relay.turnON(2, consumers);
            } else {
                relay.turnON(3, consumers);
            }
        }
    } else if (low50()) {
        relay.turnOFF(3, consumers);
        relay.turnOFF(2, consumers);
        if (needPower() < 0) {
            relay.turnON(4, consumers);
        } else {
            relay.turnOFF(4, consumers);
        }
    }
}
 

int powerConsumer3 = 0;
int powerConsumer2 = 0;

double percentageOfCharge (RelayData* consumers, RelayData wind, RelayData solar, InputAndOutput ,int batteryVoltage) {
    return floor((batteryVoltage + (0.00026 * (consumers[0].current +
                                                consumers[1].current +
                                                consumers[2].current -
                                                solar.current -
                                                wind.current -
                                                pzemClass.getA())) -
                                                7.2 ) / (12.6 - 7.2) * 100 + 0.5);
}

double timeUntilBatteryIsCharged (RelayData* consumers, RelayData wind, RelayData solar, InputAndOutput pzemClass) {
    return (2200.0*2*(0.9-percentageOfCharge(consumers, wind, solar, pzemClass, batteryVoltage)/100)/(
                                                   consumers[0].current +
                                                   consumers[1].current +
                                                   consumers[2].current -
                                                   solar.current -
                                                   wind.current -
                                                   pzemClass.getA())) * 0.7;
}

double timeUntilBatteryIsCharged (RelayData* consumers, int powerConsumerNumber, RelayData wind, RelayData solar, InputAndOutput pzemClass) {
    return (2200.0*2*(0.9-percentageOfCharge(consumers, wind, solar, pzemClass, batteryVoltage)/100)/(
                                                   consumers[0].current +
                                                   consumers[1].current +
                                                   powerConsumerNumber -
                                                   solar.current -
                                                   wind.current -
                                                   pzemClass.getA())) * 0.7;
}

double timeUntilBatteryIsCharged (RelayData* consumers, RelayData wind, RelayData solar) {
    return (2200.0*2*(0.9-percentageOfCharge(consumers, wind, solar, pzemClass, batteryVoltage)/100)/(
                                                   consumers[0].current +
                                                   consumers[1].current +
                                                   consumers[2].current -
                                                   solar.current -
                                                   wind.current)) * 0.7;
}

void MaximumPowerWork (RelayData* consumers, double timeToChargeFromServer) { //принимает сколько времени нужно для зарядки
    if (timeToChargeFromServer > timeUntilBatteryIsCharged(consumers, wind, solar, pzemClass)) {
        if (digitalRead(RELAY_2) == HIGH) {
            if (timeToChargeFromServer > timeUntilBatteryIsCharged(consumers, powerConsumer2, wind, solar, pzemClass)) {
                relay.turnON(2, consumers);
            }
        }
        if (digitalRead(RELAY_3) == HIGH) {
            if (timeToChargeFromServer > timeUntilBatteryIsCharged(consumers, powerConsumer3, wind, solar, pzemClass)) {
                relay.turnON(3, consumers);
            }
        }
        if (digitalRead(RELAY_4) == LOW) {
            if (timeToChargeFromServer > timeUntilBatteryIsCharged(consumers, wind, solar)) {
                relay.turnOFF(4, consumers);
            }
        }   
    } else {
        if (digitalRead(RELAY_4) == HIGH) {
            relay.turnON(4, consumers);
        }
        if (timeToChargeFromServer < timeUntilBatteryIsCharged (consumers, wind, solar, pzemClass)) {
            if (digitalRead(RELAY_3) == LOW) {
                powerConsumer3 = consumers[2].voltage * consumers[2].current;
                relay.turnOFF(3, consumers);
            }
        }
        if (timeToChargeFromServer < timeUntilBatteryIsCharged (consumers, wind, solar, pzemClass)) {
            if (digitalRead(RELAY_2) == LOW) {
                powerConsumer2 = consumers[1].voltage * consumers[1].current;
                relay.turnOFF(2, consumers);
            }
        } else {
            //вывод о невозможности
        }
    }
}


int low75 () { //заряд аккума больше 75%?
    double batteryVoltageESP = analogRead(35) * 3.3 / 4096 * 5.3; //настоящие вольты //флур мб убрать
    batteryVoltage = batteryVoltageESP + (0.00028 * (1 + 0.0001 * (batteryVoltageESP - 12.5)) * (consumers[0].current + consumers[1].current + consumers[2].current - solar.current / 2 - wind.current / 2 - pzemClass.getA() * 4)); //мнимые вольты

    // Serial.println(batteryVoltage);

    return (((batteryVoltage-7.2) / (14 - 7.2)) < 0.75) ? 1 : 0;
}


//перед началом работы алгоритма надо включить реле
  
void MaximumReliabilityWork(RelayData* consumers, RelayData wind, RelayData solar, InputAndOutput pzemClass) {

    Serial.print("low50");
    Serial.println(low50());
    Serial.print ("consumption_forecast(consumers, wind, solar, pzemClass)");
    Serial.println(consumption_forecast(consumers, wind, solar, pzemClass));
    Serial.print("needPower()");
    Serial.println(needPower());

    if (!consumption_forecast(consumers, wind, solar, pzemClass)) {
            relay.turnOFF(3, consumers);
            relay.turnON(4, consumers);
        if (!consumption_forecast(consumers, wind, solar, pzemClass)) {
            relay.turnOFF(2, consumers);
            relay.turnON(4, consumers);

        } else if(!low75()){
            relay.turnON(2, consumers);
            relay.turnOFF(4, consumers);
        } else { 
            relay.turnON(4, consumers); 
        }
    } else if (!low75()) {
        relay.turnON(3, consumers);
        relay.turnOFF(4, consumers);
    } else {
        relay.turnON(4, consumers);
    }
}

// void надёжность () {

// }

enum unit {
    VOLTAGE = 0,
    CURRENT
};

void prepSensorsData (double tmp, RelayData* consumers, int relayNumber, int unit) {

    if (unit == CURRENT) {
        if (tmp > 0.01) {
            consumers[relayNumber].current = tmp;
        } else {
            consumers[relayNumber].current = 0;
        }
    } else if (unit == VOLTAGE) {
        if (tmp > 2) {
            consumers[relayNumber].voltage = tmp;
        } else {
            consumers[relayNumber].voltage = 0;
        }
    }
}

void prepManufacturersData (double tmp, RelayData& manufacturers, int unit) {
    if (unit == CURRENT) {
        if (tmp > 0.01) {
            manufacturers.current = tmp;
        } else {
            manufacturers.current = 0;
        }
    } else if (unit == VOLTAGE) {
        if (tmp > 2) {
            manufacturers.voltage = tmp;
        } else {
            manufacturers.voltage = 0;
        }
    }
}

void setup() {
    Serial.begin(115200);

    // Установка адреса модуля PZEM004T
    pzem.setAddress(pzemIP);

    //Инициализация wifi
    WifiInit();
}

long ms = 0;

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

    bool loginSuc = false;

    int MaximumReliabilityRelayTurnOn = 0;

    //решить с Эдиком

    if (!loginSuc) {
        char* login = "priority2030";
        char* pass = "12345";
        auto [auth, counter] = encodeAuth(login, pass);
        for (int i = 0; i < counter; i++) {
            client.print(*auth);
            auth++;
        }
        loginSuc = true;
        // Serial.println("login succsess");
    }

    // Получение статусов модуля реле
    consumers[0].status = (digitalRead(RELAY_1) == LOW) ? 1 : 0;
    consumers[1].status = (digitalRead(RELAY_1) == LOW) ? 1 : 0;
    consumers[2].status = (digitalRead(RELAY_1) == LOW) ? 1 : 0;

    //инициализация I2C и INA
    TwoWire I2Cone = TwoWire(0);
    TwoWire I2Ctwo = TwoWire(1);

    I2Cone.begin(SDA_1, SCL_1);
    I2Ctwo.begin(SDA_2, SCL_2);

    ina219_A1.begin (&I2Cone);
    ina219_A2.begin (&I2Cone);
    ina219_A3.begin (&I2Cone);
    ina219_B1.begin (&I2Ctwo);
    ina219_B2.begin (&I2Ctwo);

    ina219_A1.powerSave(0);
    ina219_A2.powerSave(0);
    ina219_A3.powerSave(0);
    ina219_B1.powerSave(0);
    ina219_B2.powerSave(0);

    //буффер для обработки запросов с сервера
    static char buffer[MSG_BUFF_SIZE];
    int workMode = 0; //режим работы (ручной или любой другой)

    if (loginSuc) {
    //пока сервер подключен
        while (client.connected()) {

            // Serial.println("kek");

            if (millis() - ms >= 3000) {

                #ifdef DEBUG
                Serial.println(generator.current);
                Serial.println(generator.voltage);
                #endif
                

                // batteryVoltage = analogRead(35)*3.3/4096*5.25;
                // Serial.println(batteryVoltage);

                if (millis() - timerBattery > 30000 || !isSendBattaryVoltage) {
                    timerBattery = millis();
                    double batteryVoltageESP = analogRead(35) * 3.3 / 4096 * 5.3; //настоящие вольты //флур мб убрать
                    batteryVoltage = batteryVoltageESP + (0.00028 * (1 + 0.0001 * (batteryVoltageESP - 12.5)) * (consumers[0].current + consumers[1].current + consumers[2].current - solar.current / 2 - wind.current / 2 - pzemClass.getA() * 4)); //мнимые вольты
                    isSendBattaryVoltage = true;
                }

                Serial.println(batteryVoltage);

                int tmp_batteryVoltageToServer = analogRead(35);

                if (tmp_batteryVoltageToServer < 1500) {
                    batteryVoltageToServer = 0;
                } else {
                    batteryVoltageToServer = tmp_batteryVoltageToServer;
                }

                prepSensorsData(ina219_A1.getBusVoltage_V(), consumers, 0, VOLTAGE);
                prepSensorsData(ina219_A2.getBusVoltage_V(), consumers, 1, VOLTAGE);
                prepSensorsData(ina219_A3.getBusVoltage_V(), consumers, 2, VOLTAGE);

                prepSensorsData(ina219_A1.getCurrent_mA() / 1000, consumers, 0, CURRENT);
                prepSensorsData(ina219_A2.getCurrent_mA() / 1000, consumers, 1, CURRENT);
                prepSensorsData(ina219_A3.getCurrent_mA() / 1000, consumers, 2, CURRENT);

                prepManufacturersData(ina219_B1.getBusVoltage_V(), wind, VOLTAGE);
                prepManufacturersData(ina219_B2.getBusVoltage_V(), wind, VOLTAGE);
                prepManufacturersData(pzem.voltage(pzemIP), generator, VOLTAGE);
                pzemClass.setV(generator.voltage);

                prepManufacturersData(ina219_B1.getCurrent_mA() / 1000, solar, CURRENT);
                prepManufacturersData(ina219_B2.getCurrent_mA() / 1000, wind, CURRENT);
                prepManufacturersData(pzem.current(pzemIP), generator, CURRENT);
                pzemClass.setA(generator.current);

                std::pair<char*, int> result = encodeSensorsData(solar, wind, generator, batteryVoltageToServer, consumers);
                char* AlreadyDataToServer = result.first;
                int counterData = result.second;

                for (int i = 0; i < counterData; i++) {
                    client.print(*AlreadyDataToServer++);
                } 
                ms = millis();

            }

            int size = 0;
            size = client.available();
            int size_check = size;
            int i = 0;
            Serial.println(size);

            while (size) {
                #ifdef DEBAG
                Serial.println("Some message from server");
                #endif
                buffer[i] = client.read();
                --size;
                ++i;
            }

            Datagram* d = reinterpret_cast<Datagram*>(buffer);
            if (size_check != 0) {
                switch (d->type) {
                case MODE: {
                    Mode* modeData = reinterpret_cast<Mode*>(d + 1);

                    #ifdef DEBUG
                    Serial.println("switch mode check");
                    #endif

                                                                        Serial.println(modeData->factor);

                    switch (modeData->mode) {
                    case Manual: {
                        workMode = 0;
                        #ifdef DEBAG
                        Serial.println("manual work mode");
                        #endif
                        break;
                    }
                    case MaximumReliability: {
                        workMode = 1;
                        #ifdef DEBAG
                        Serial.println("Maximum reliability work mode");
                        #endif
                        Serial.println("check31");
                        break;
                    }
                    case MaximumPower: {
                        workMode = 2;
                        #ifdef DEBAG
                        Serial.println("Maximum power work mode");
                        #endif
                        break;
                    }
                    case MaximumEcology: {
                        workMode = 3;
                        #ifdef DEBAG
                        Serial.println("Maximum ecology work mode");
                        #endif
                        break;
                    }
                    case MaximumEconomy:{
                        workMode = 4;
                        #ifdef DEBAG
                        Serial.println("Maximum economy work mode");
                        #endif
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
                    Serial.println("Switch reley");

                    if (relayData->status == 0) {
                        relay.turnOFF (relayData->relayNumber, consumers);
                    } else if (relayData->status == 1) {
                        relay.turnON (relayData->relayNumber, consumers);
                    }
                    
                    #ifdef DEBAG
                    Serial.print("relay number ");
                    Serial.println(relayData->relayNumber);
                    Serial.print("relay status ");
                    Serial.println(relayData->status);
                    #endif

                    break;
                }
                case SENSORS_DATA: {   

                    break;
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
                // MaximumReliabilityRelayTurnOn = 0;
                break;
            }
            case 1: {
                if (MaximumReliabilityRelayTurnOn == 0) {
                    relay.turnON(1, consumers);
                    relay.turnON(2, consumers);
                    relay.turnON(3, consumers);
                    relay.turnON(4, consumers);
                    MaximumReliabilityRelayTurnOn = 1;
                }
                MaximumReliabilityWork(consumers, wind, solar, pzemClass);
                break;
            }
            case 2: {
                // MaximumPower
                MaximumReliabilityRelayTurnOn = 0;
                MaximumPowerWork(consumers, 1000000);
                break;
            }
            case 3: {
                //MaximumEcology
                MaximumReliabilityRelayTurnOn = 0;
                ecoWork();
                break;
            }
            case 4: {
                //MaximumEconomy
                MaximumReliabilityRelayTurnOn = 0;
                economicalWork();
                delay(200);
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
    delay (200);
    
}