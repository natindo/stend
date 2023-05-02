#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>
#include <string>
#define _GLIBCXX_USE_CXX11_ABI 0

class Relay {
public:
    Relay(uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4);
    std::string relayHandler(int relayNum, char *relayCommand);
    std::string getStatus (int relayNum);
    std::string turnON(int relayNum);
    std::string turnOFF(int relayNum);
    std::string switchState(int relayNum);

private:
    uint8_t _relay1, _relay2, _relay3, _relay4;
};

#endif // RELAY_H
