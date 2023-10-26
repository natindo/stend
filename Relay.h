#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>
#include <string>
#include "esp_structs.h"
#define _GLIBCXX_USE_CXX11_ABI 0

class Relay {
public:
    Relay(uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4, uint8_t rmeasured);
    void turnON(int relayNum, RelayData consumers[RELAYS_NUMBER]);
    void turnOFF(int relayNum, RelayData consumers[RELAYS_NUMBER]);

private:
    uint8_t _relay1, _relay2, _relay3, _relay4, _relaymeasured;
};

#endif // RELAY_H
