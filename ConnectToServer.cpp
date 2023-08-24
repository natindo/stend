#include "lib.h"

// Название и пароль точки доступа
// const char *ssid = "IU4-net";
// const char *password = "iu4wifivip";

const char *ssid = "esp32";
const char *password = "qwerty1234";

//const char *ssid = "Natindo";
//const char *password = "jb7niwhyxv66q";

// Сетевой порт и IP-адрес сервера
const uint16_t port = 50000;
IPAddress serverIP(192, 168, 88, 212);

void WifiInit () {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println(WiFi.status());
}

// Подключение к WiFi 
