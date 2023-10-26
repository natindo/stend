#ifndef ESP_STRUCTS_H
#define ESP_STRUCTS_H

#include <iostream>
#include <memory>

const size_t RELAYS_NUMBER = 3;

enum DatagramType {
    SUCCESS = 0,
    ERROR,
    AUTH,
    SENSORS_DATA,
    SWITCH_RELAY = 5,
    MODE
};

enum ModeType {
    Manual = 0,
    MaximumReliability,
    MaximumPower,
    MaximumEcology,
    MaximumEconomy
};

struct Datagram {
    int type;
    int size;
};

struct Auth {
    int loginSize;
    int passwordSize;
};

struct Mode {
    ModeType mode;
    double factor;
};

struct SwitchRelay {
    int relayNumber;
    bool status;
};

struct RelayData {
    double voltage;
    double current;
    bool status;
};

struct SensorsData {
    RelayData solar;
    RelayData wind;
    RelayData generator;
    double battery_voltage;
    RelayData consumer[RELAYS_NUMBER];
};

int get_size_of_type(DatagramType type);

template<typename TStruct>
TStruct get_struct(std::shared_ptr<char[]> buffer);

#endif ESP_STRUCTS_H