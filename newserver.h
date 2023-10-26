
#include "esp_structs.h"
#include <memory>
#include <string.h>
#include <utility>


// возвращает char* выделенной памяти !НЕ ЗАБЫТЬ ПОЧИСТИТЬ!
// возвращает сообщение о невозможности выполнить операцию
std::pair<char*, int> errorMessage ();

// функция принимает аргументами структуры RelayData, которые заполняются просто установкой полей структуры
// возвращает char* выделенной памяти !НЕ ЗАБЫТЬ ПОЧИСТИТЬ!
std::pair<char*, int> encodeSensorsData(RelayData solar, RelayData wind, RelayData generator, double batteryVoltage, RelayData consumers[RELAYS_NUMBER]);

// функция принимает логин и пароль, возвращает также буфер, который необходимо отправить
// !НЕ ЗАБЫТЬ ПОЧИСТИТЬ!
std::pair<char*, int> encodeAuth(char* login, char* password);
