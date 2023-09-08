#include "esp32-hal-gpio.h"
#include "Relay.h"
#include <string>
#define _GLIBCXX_USE_CXX11_ABI 0

extern int relaySwitchCounter;

void stopRelaySwitching () {
    if (++relaySwitchCounter > 5) {
        relaySwitchCounter = 0;
        delay(2000);
    }
}

Relay::Relay(uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4) : _relay1(r1), _relay2(r2), _relay3(r3), _relay4(r4) {
    pinMode(_relay1, OUTPUT);
    pinMode(_relay2, OUTPUT);
    pinMode(_relay3, OUTPUT);
    pinMode(_relay4, OUTPUT);
    digitalWrite(_relay1, HIGH);
    digitalWrite(_relay2, HIGH);
    digitalWrite(_relay3, HIGH);
    digitalWrite(_relay4, HIGH);
}

void Relay::turnON(int relayNum, RelayData consumers[RELAYS_NUMBER]) {
    stopRelaySwitching ();
    switch (relayNum) {
    case 1:
        digitalWrite(_relay1, LOW);
        consumers[0].status = 1;
        break;
    case 2:
        digitalWrite(_relay2, LOW);
        consumers[1].status = 1;
        break;
    case 3:
        digitalWrite(_relay3, LOW);
        consumers[2].status = 1;
        break;
    case 4:
        digitalWrite(_relay4, LOW);
        consumers[3].status = 1;
        break;
    }
}

void Relay::turnOFF(int relayNum, RelayData consumers[RELAYS_NUMBER]) {
    stopRelaySwitching ();
    switch (relayNum) {
    case 1:
        digitalWrite(_relay1, HIGH);
        consumers[0].status = 0;
        break;
    case 2:
        digitalWrite(_relay2, HIGH);
        consumers[1].status = 0;
        break;
    case 3:
        digitalWrite(_relay3, HIGH);
        consumers[2].status = 0;
        break;
    case 4:
        digitalWrite(_relay4, HIGH);
        consumers[3].status = 0;
        break;
    }
}
