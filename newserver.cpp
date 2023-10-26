// функция принимает буфер, в котором содержатся пришедшие данные
// изначально можно считать sizeof(Datagram) байтов, чтобы понять тип пришедшего сообщения
// также записан размер полезной нагрузки (возможно его стоит выпилить, так как сообщения имеют постоянный размер)
// после чтения первых sizeof(Datagram) байтов, понимаем, что записано дальше и читаем необходимое число байтов

#include "esp_structs.h"
#include <memory>
#include <string.h>
#include <utility>
#include "lib.h"

std::pair<char*, int> errorMessage () {
    
    Datagram d;
    d.type = ERROR;

    char* buff = new char[sizeof(Datagram)];

    memcpy(buff, &d, sizeof(Datagram));

    return {buff, sizeof(Datagram)};
}   

// функция принимает аргументами структуры RelayData, которые заполняются просто установкой полей структуры
// возвращает char* выделенной памяти !НЕ ЗАБЫТЬ ПОЧИСТИТЬ!
std::pair<char*, int> encodeSensorsData(RelayData solar, RelayData wind, RelayData generator, double batteryVoltage, RelayData consumers[RELAYS_NUMBER]) {

    int dataSize = 0;

    Datagram d;
    d.type = SENSORS_DATA;
    //d.size = sizeof(payload);

    // Serial.println("checkencode1");

    dataSize += sizeof(Datagram);

    SensorsData payload;
    payload.solar = solar;
    payload.wind = wind;
    payload.generator = generator;
    payload.battery_voltage = batteryVoltage;

    Serial.println(payload.battery_voltage);

    for (int i = 0; i < RELAYS_NUMBER; ++i) {
        payload.consumer[i] = consumers[i];
    }

    dataSize += sizeof(SensorsData);
    d.size = sizeof(SensorsData);

    char* buff = new char[sizeof(Datagram) + sizeof(SensorsData)];

    memcpy(buff, &d, sizeof(Datagram));
    memcpy(buff + sizeof(Datagram), &payload, sizeof(SensorsData));

    return {buff, dataSize};
}

// функция принимает логин и пароль, возвращает также буфер, который необходимо отправить
// !НЕ ЗАБЫТЬ ПОЧИСТИТЬ!
std::pair <char*, int> encodeAuth(char* login, char* password) {
    int dataSize = 0;
    
    Datagram d;
    d.type = AUTH;
    d.size = sizeof(Auth);
    
    dataSize += sizeof(Datagram);

    Auth payload;
    payload.loginSize = strlen(login);
    payload.passwordSize = strlen(password);

    dataSize += payload.loginSize;
    dataSize += payload.passwordSize;
    dataSize += sizeof(Auth);

    Serial.println(payload.loginSize);
    Serial.println(payload.passwordSize);

    char* buff = new char[dataSize];

    memcpy(buff, &d, sizeof(Datagram));
    memcpy(buff + sizeof(Datagram), &payload, sizeof(Auth));
    memcpy(buff + sizeof(Datagram) + sizeof(Auth), login, payload.loginSize);
    memcpy(buff + sizeof(Datagram) + sizeof(Auth) + payload.loginSize, password, payload.passwordSize);

    return {buff, dataSize};
}
