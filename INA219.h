#ifndef INA219_H
#define INA219_H
#include "lib.h"
class Ina {
public:
    Ina ();
    ~Ina();

    void initI2CAndIna (Adafruit_INA219 &ina219_A1, Adafruit_INA219 &ina219_A2, Adafruit_INA219 &ina219_A3, Adafruit_INA219 &ina219_B1, Adafruit_INA219 &ina219_B2);
    void getVoltage (Adafruit_INA219 &ina219_A1, Adafruit_INA219 &ina219_A2, Adafruit_INA219 &ina219_A3, Adafruit_INA219 &ina219_B1, Adafruit_INA219 &ina219_B2);
    void getCurrent (Adafruit_INA219 &ina219_A1, Adafruit_INA219 &ina219_A2, Adafruit_INA219 &ina219_A3, Adafruit_INA219 &ina219_B1, Adafruit_INA219 &ina219_B2);

    float shuntvoltageA1;
    float shuntvoltageA2;
    float shuntvoltageA3;
    float shuntvoltageB1;
    float shuntvoltageB2;
    float currentA1;
    float currentA2;
    float currentA3;
    float currentB1;
    float currentB2;
};

#endif // INA219_H
