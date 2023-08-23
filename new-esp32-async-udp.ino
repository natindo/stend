#include "lib.h"
#include <WiFi.h>
#include <cstring>
#include <thread>
#include "esp_structs.h"
#include "newserver.h"
#include <utility>
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

//объекты измерителей тока и напряжения
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


RelayData solar;
RelayData wind;
RelayData generator;
RelayData consumers[RELAYS_NUMBER];
int batteryVoltage;


// Объект модуля с четырьмя реле
Relay relay(RELAY_1, RELAY_2, RELAY_3, RELAY_4);

// Объекты для работы с модулем измерения переменного тока и напряжения
HardwareSerial PzemSerial2(2);     // Use hwserial UART2 at pins IO-16 (RX2) and IO-17 (TX2)
PZEM004T pzem(&PzemSerial2);
IPAddress pzemIP(192, 168, 1, 1);

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
    return (12.4-(analogRead(35)*3.3/4096*5)/(12.4-7.2)<0.5) ?1:0;
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
        if (!needPower() && digitalRead(RELAY_4) == HIGH) {
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

// void xvataetLiKZadannomuChasy (int ) {

// }

// void  () {

// }

// void MaximumPowerWork () { //СДЕЛАТЬ ПРОСЧЁТ МОЩНОСТИ В ОТДЕЛЬНОЙ ФУНКЦИИ!!!!!!
//     //приходит сколько может быть 
//     if (/*свободной мощности хватает*/) {
//         if (/*!включена 2 группа*/) {
//             if (/*акб зарядится с учётом потребления*/) {
//                 //включить 2 группу
//             }
//         }
//         if (/*!включена 3 группа*/) {
//             if (/*акб зарядится с учётом потребления*/) {
//                 //включить 3 группу
//             }
//         }
//         if (/*Включён дизель*/) {
//             if (/*акб зарядиться к нужному часу без дизеля*/) {
//                 //отключить дизель
//             }
//         }   
//     } else {
//         if (/*выключен дизель*/) {
//             //включить дизель
//         }
//         if (/*не хватает свободной мощности для заряда к заданному часу*/) {
//             if (/*включена 3 группа потребителей*/) {
//                 //сохранить значение мощности
//                 //отключить 3 группу
//             }
//         }
//         if (/*не хватает свободной мощности для заряда к заданному часу*/) {
//             if (/*включена 2 группа*/) {
//                 //сохранить значение мощности
//                 //отключить 2 группу
//             }
//         } else {
//             //вывод о невозможности
//         }
//     }
// }

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT); // настройства светодиода для индикации наличия авторизации с сервером
    delay(100);

    // Установка адреса модуля PZEM004T
    pzem.setAddress(pzemIP);

    //Инициализация wifi
    WifiInit();
}

void loop() {
    //попытка законнектиться к серверу
    while (!isConnectToServer) {
        //connect to server
        const uint16_t port = 2002; // port TCP server
        const char * host = "192.168.88.232"; // ip or dns
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
    char* login = "priority2030";
    char* pass = "12345";
    auto [auth, counter] = encodeAuth(login, pass);
    for (int i = 0; i < counter; i++) {
        client.print(*auth);
        Serial.println(*auth);
        auth++;
    }

    //client.print(auth);

    // Получение статусов модуля реле
    groupOut1.status = (digitalRead(RELAY_1) == LOW) ? 1 : 0;
    groupOut2.status = (digitalRead(RELAY_2) == LOW) ? 1 : 0;
    groupOut3.status = (digitalRead(RELAY_3) == LOW) ? 1 : 0;

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
    uint16_t ms = 0; //секунд 5
    int workMode = 0; //режим работы (ручной или любой другой)

    //пока сервер подключен
    while (client.connected()) {

        Serial.println("check1");

        if (millis() - ms >= 5000) {

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
            batteryVoltage = analogRead(35)*3.3/4096*5;

            consumers[0].voltage = ina219_A1.getBusVoltage_V();
            consumers[1].voltage = ina219_A2.getBusVoltage_V();
            consumers[2].voltage = ina219_A3.getBusVoltage_V();
            consumers[0].current = ina219_A1.getCurrent_mA();
            consumers[1].current = ina219_A2.getCurrent_mA();
            consumers[2].current = ina219_A3.getCurrent_mA();

            //решить с Эдиком
            //char AlreadyDataToServer[200];
            auto [AlreadyDataToServer, counter_data] = encodeSensorsData(solar, wind, generator, batteryVoltage, consumers);
            for (int i = 0; i < counter_data; i++) {
                client.print(AlreadyDataToServer++);
            } 
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
            Serial.println(buffer[i]);
            --size;
            ++i;
        }

        Datagram* d = reinterpret_cast<Datagram*>(buffer);
        if (size_check != 0) {
            //Serial.println(size_check);
            switch (d->type) {
            case MODE: {
                Mode* modeData = reinterpret_cast<Mode*>(d + 1);

                switch (modeData->mode) {
                case Manual: {
                    workMode = 0;
                    break;
                }
                case MaximumReliability: {
                    workMode = 1;
                    break;
                }
                case MaximumPower: {
                    workMode = 2;
                    break;
                }
                case MaximumEcology: {
                    workMode = 3;
                    break;
                }
                case MaximumEconomy:{
                    workMode = 4;
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
                batteryVoltage = analogRead(35)*3.3/4096*5;

                consumers[0].voltage = ina219_A1.getBusVoltage_V();
                consumers[1].voltage = ina219_A2.getBusVoltage_V();
                consumers[2].voltage = ina219_A3.getBusVoltage_V();
                consumers[0].current = ina219_A1.getCurrent_mA();
                consumers[1].current = ina219_A2.getCurrent_mA();
                consumers[2].current = ina219_A3.getCurrent_mA();

                //решить с Эдиком
                //char AlreadyDataToServer[200];
                auto [AlreadyDataToServer, counter_data] = encodeSensorsData(solar, wind, generator, batteryVoltage, consumers);
                for (int i = 0; i < counter_data; i++) {
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
            Serial.println("check11");
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
            break;
        }
        case 3: {
            //MaximumEcology
            Serial.println("check14");
            ecoWork();
            break;
        }
        case 4: {
            //MaximumEconomy
            Serial.println("check15");
            economicalWork();
            break;
        }
        }

        for (int i = 0; i < MSG_BUFF_SIZE; ++i) {
            buffer[i] = 9;
        }

    }

    //если вышли из предыдущего вайла - коннект потерян
    isConnectToServer = false;
    isAuthToServer = false;
    isAuthFromServer = false;

    Serial.println("");
    delay (200);
    
}
