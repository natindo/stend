#include "lib.h"
#include <WiFi.h>
#include <cstring>
#include <thread>
#include "esp_structs.h"
#include "newserver.h"
#include <utility>
#include <math.h>
// #define DEBAG
//kek
//Волк
// `sfbgzfd
// bxzcv
// b
// cxv
// bzcx
// vb
// xcv
// bzcv

#define BatteryPin 35

#define RELAY_1 26
#define RELAY_2 25
#define RELAY_3 33
#define RELAY_4 32
#define RELAY_MEASURED 14

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

int factor = 0;

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
Relay relay(RELAY_1, RELAY_2, RELAY_3, RELAY_4, RELAY_MEASURED);

// Объекты для работы с модулем измерения переменного тока и напряжения
HardwareSerial PzemSerial2(2);     // Use hwserial UART2 at pins IO-16 (RX2) and IO-17 (TX2)
PZEM004T pzem(&PzemSerial2);
IPAddress pzemIP(192, 168, 1, 1);

void measuredBattary () {
    digitalWrite(RELAY_MEASURED, LOW);
    delay(1000);
    batteryVoltage = analogRead(35) * 3.3 / 4096 * 5.3;
    delay(1000);
    Serial.println(batteryVoltage);
    digitalWrite(RELAY_MEASURED, HIGH);
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

    return (solar.voltage * solar.current + wind.voltage * wind.current + pzemClass.getA() * 1000 * pzemClass.getV()*0.5 - (consumers[0].voltage * consumers[0].current +
    consumers[1].voltage * consumers[1].current + consumers[2].voltage * consumers[2].current));
}

int low (double percent) {

    return (((batteryVoltage-7.2) / (14 - 7.2)) < percent) ? 1 : 0;

}

void ecoWork (int factor) {

    if (factor > 1) {
        if (!low(0.5)) {
            if (!low(0.95)) {
                relay.turnOFF(4, consumers);
                relay.turnON(2, consumers);
                relay.turnON(3, consumers);
            } else if (low(0.92)) {
                relay.turnOFF(4, consumers);
                relay.turnON(2, consumers);
                if (needPower() > 7500) {
                    relay.turnON(3, consumers);
                } else if (needPower() < -10000) {
                    relay.turnOFF(3, consumers);
                }
            }
        } else if (low(0.47)){
            if (!low(0.2)) {
                relay.turnOFF(4, consumers);
                relay.turnOFF(3, consumers);
                if (needPower() > 7500) {
                    relay.turnON(2, consumers);
                } else if (needPower() < -10000) {
                    relay.turnOFF(2, consumers);
                }
            } else if (low(0.17)) {
                relay.turnON(4, consumers);
                relay.turnOFF(2, consumers);
                relay.turnOFF(3, consumers);
            }
        }
    } else {
        if (!low(0.5)) {
            if (!low(0.95)) {
                relay.turnOFF(4, consumers);
                if (needPower() > 7500) {
                    relay.turnON(2, consumers);
                    relay.turnON(3, consumers);
                } else if (needPower() < -10000) {
                    relay.turnOFF(2, consumers);
                    relay.turnOFF(3, consumers);
                }
            } else if (low(0.92)) {
                relay.turnON(4, consumers);
                relay.turnOFF(3, consumers);
                if (needPower() > 7500) {
                    relay.turnON(2, consumers);
                } else if (needPower() < -10000) {
                    relay.turnOFF(2, consumers);
                }
            }
        } else if (low(0.47)) {
            relay.turnON(4, consumers);
            relay.turnOFF(3, consumers);
            relay.turnOFF(2, consumers);
        }
    }

}

void economicalWork (int factor) {
    Serial.println(needPower());
    if (factor >= 1) {
        if (!low(0.5)) {
            if (!low(0.95)) {
                relay.turnOFF(4, consumers);
                relay.turnON(2, consumers);
                relay.turnON(3, consumers);
            } else if (low(0.92)) {
                relay.turnOFF(4, consumers);
                relay.turnON(2, consumers);
                if (needPower() > 7500) {
                    relay.turnON(3, consumers);
                } else if (needPower() < -10000) {
                    relay.turnOFF(3, consumers);
                }
            }
        } else if (low(0.47)) {
            if (!low(0.2)) {
                relay.turnOFF(4, consumers);
                relay.turnOFF(3, consumers);
                if (needPower() > 7500) {
                    relay.turnON(2, consumers);
                } else if (needPower() < -10000) {
                    relay.turnOFF(2, consumers);
                }
            } else if (low(0.17)) {
                relay.turnON(4, consumers);
                relay.turnOFF(2, consumers);
                relay.turnOFF(3, consumers);
            }
        }
    } else {
        if (!low(0.5)) {
            if (!low(0.95)) {
                relay.turnON(4, consumers);
                if (needPower() > 7500) {
                    if (digitalRead(RELAY_2) == HIGH) {
                        relay.turnON(2, consumers);
                    } else {
                        relay.turnON(3, consumers);
                    }
                } else if (needPower() < -10000) {
                    if (digitalRead(RELAY_3) == LOW) {
                        relay.turnOFF(3, consumers);
                    } else {
                        relay.turnOFF(2, consumers);
                    }
                }
            } else if (low(0.92)) {
                relay.turnON(4, consumers);
                relay.turnOFF(3, consumers);
                if (needPower() > 7500) {
                    relay.turnON(2, consumers);
                } else if (needPower() < -10000) {
                    relay.turnOFF(2, consumers);
                }
            }
        } else if (low(0.47)) {
            relay.turnON(4, consumers);
            relay.turnOFF(3, consumers);
            relay.turnOFF(2, consumers);
        }
    }

}

void MaximumPowerWork (RelayData* consumers, int factor) {
    if (factor > 1) {
        if (!low(0.5)) {
            if (!low(0.95)) {
                relay.turnOFF(4, consumers);
                relay.turnON(2, consumers);
                relay.turnON(3, consumers);
            } else if (low(0.92)) {
                if (needPower() <= 0) {
                    relay.turnON(4, consumers);
                }
                relay.turnON(2, consumers);
                relay.turnON(3, consumers);
            }
        } else if (low(0.47)) {
            if (!low(0.2)) {
                if (needPower() <= 0) {
                    relay.turnON(4, consumers);
                }
                relay.turnON(2, consumers);
                relay.turnON(3, consumers);
            } else if (low(0.17)) {
                relay.turnON(4, consumers);
                relay.turnON(2, consumers);
                if (needPower() > 7500) {
                    relay.turnON(3, consumers);
                } else if (needPower() < -10000) {
                    relay.turnOFF(3, consumers);
                }
            }
        }
    } else {
        if (!low(0.5)) {
            if (!low(0.95)) {
                relay.turnOFF(4, consumers);
                relay.turnON(2, consumers);
                relay.turnON(3, consumers);
            } else if (low(0.92)) {
                if (needPower() <= 0) {
                    relay.turnON(4, consumers);
                }
                relay.turnON(2, consumers);
                relay.turnON(3, consumers);
            }
        } else if (low(0.47)) {
            relay.turnON(4, consumers);
                if (needPower() > 7500) {
                    relay.turnON(2, consumers);
                    relay.turnON(3, consumers);
                } else if (needPower() < -10000) {
                    relay.turnOFF(2, consumers);
                    relay.turnOFF(3, consumers);
                }
        } 
    }
}

void MaximumReliabilityWork(RelayData* consumers, int factor) {
    if (factor > 1) {
        if (!low(0.5)) {
            if (!low(0.95)) {
                relay.turnOFF(4, consumers);
                relay.turnON(2, consumers);
                relay.turnON(3, consumers);
            } else if (low(0.92)) {
                relay.turnON(4, consumers);
                relay.turnON(2, consumers);
                if (needPower() > 7500) {
                    relay.turnON(3, consumers);
                } else if (needPower() < -10000) {
                    relay.turnOFF(3, consumers);
                }
            }
        } else if (low(0.47)) {
            relay.turnON(4, consumers);
            if (needPower() > 7500) {
                relay.turnON(2, consumers);
                relay.turnON(3, consumers);
            } else if (needPower() < -10000) {
                relay.turnOFF(2, consumers);
                relay.turnOFF(3, consumers);
            }
        }
    } else {
        if (!low(0.5)) {
            if (!low(0.95)) {
                relay.turnOFF(4, consumers);
                relay.turnON(2, consumers);
                relay.turnON(3, consumers);
            } else if (low(0.92)) {
                relay.turnON(4, consumers);
                if (needPower() > 7500) {
                    relay.turnON(2, consumers);
                    relay.turnON(3, consumers);
                } else if (needPower() < -10000) {
                    relay.turnOFF(2, consumers);
                    relay.turnOFF(3, consumers);
                }
            }
        } else if (low(0.47)) {
            relay.turnON(4, consumers);
            if (needPower() > 7500) {
                relay.turnON(2, consumers);
                relay.turnON(3, consumers);
            } else if (needPower() < -10000) {
                relay.turnOFF(2, consumers);
                relay.turnOFF(3, consumers);
            }
        }
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

    pinMode(14, OUTPUT);

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
        const char * host = "192.168.3.8"; // ip or dns
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
    consumers[1].status = (digitalRead(RELAY_2) == LOW) ? 1 : 0;
    consumers[2].status = (digitalRead(RELAY_3) == LOW) ? 1 : 0;

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

    relay.turnON(1, consumers);

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
                    measuredBattary();
                    isSendBattaryVoltage = true;
                }

                prepSensorsData(int (ina219_A1.getBusVoltage_V() * 100)/100.0, consumers, 0, VOLTAGE);
                prepSensorsData(int (ina219_A2.getBusVoltage_V() * 100)/100.0, consumers, 1, VOLTAGE);
                prepSensorsData(int (ina219_A3.getBusVoltage_V() * 100)/100.0, consumers, 2, VOLTAGE);

                prepSensorsData(int ((ina219_A1.getCurrent_mA() / 1000) * 100)/100.0, consumers, 0, CURRENT);
                prepSensorsData(int ((ina219_A2.getCurrent_mA() / 1000) * 100)/100.0, consumers, 1, CURRENT);
                prepSensorsData(int ((ina219_A3.getCurrent_mA() / 1000) * 100)/100.0, consumers, 2, CURRENT);

                prepManufacturersData(int (ina219_B1.getBusVoltage_V() * 100)/100.0, wind, VOLTAGE);
                prepManufacturersData(int (ina219_B2.getBusVoltage_V() * 100)/100.0, wind, VOLTAGE);
                prepManufacturersData(int (pzem.voltage(pzemIP) * 100)/100.0, generator, VOLTAGE);
                pzemClass.setV(generator.voltage);

                prepManufacturersData(int ((ina219_B1.getCurrent_mA() / 1000) * 100)/100.0, solar, CURRENT);
                prepManufacturersData(int ((ina219_B2.getCurrent_mA() / 1000) * 100)/100.0, wind, CURRENT);
                prepManufacturersData(int ((pzem.current(pzemIP))* 100)/100.0, generator, CURRENT);
                pzemClass.setA(generator.current);

                double batteryVoltageToServer = int (batteryVoltage * 10) / 10.0;

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
            //Serial.println(size);

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

                    factor = modeData->factor;

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
                    // Serial.println("Switch reley");

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
                MaximumReliabilityRelayTurnOn = 0;
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
                MaximumReliabilityWork(consumers, factor);
                break;
            }
            case 2: {
                // MaximumPower
                MaximumReliabilityRelayTurnOn = 0;
                MaximumPowerWork(consumers, factor);
                break;
            }
            case 3: {
                //MaximumEcology
                MaximumReliabilityRelayTurnOn = 0;
                ecoWork(factor);
                break;
            }
            case 4: {
                //MaximumEconomy
                MaximumReliabilityRelayTurnOn = 0;
                economicalWork(factor);
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