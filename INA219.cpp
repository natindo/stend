#include "lib.h"
#define _GLIBCXX_USE_CXX11_ABI 0

#define SDA_1 5
#define SCL_1 18

#define SDA_2 19
#define SCL_2 21

Ina::Ina () {
    this->currentA1 = 0;
    this->currentA2 = 0;
    this->currentA3 = 0;
    this->currentB1 = 0;
    this->currentB2 = 0;
    this->shuntvoltageA1 = 0;
    this->shuntvoltageA2 = 0;
    this->shuntvoltageA3 = 0;
    this->shuntvoltageB1 = 0;
    this->shuntvoltageB2 = 0;   
}

Ina::~Ina () {
    Serial.println ("end work");
}

void Ina::initI2CAndIna (Adafruit_INA219 &ina219_A1, Adafruit_INA219 &ina219_A2, Adafruit_INA219 &ina219_A3, Adafruit_INA219 &ina219_B1, Adafruit_INA219 &ina219_B2) {
    TwoWire I2Cone = TwoWire(0);
    TwoWire I2Ctwo = TwoWire(1);

    I2Cone.begin(SDA_1, SCL_1);
    I2Ctwo.begin(SDA_2, SCL_2);

   ina219_A1.begin (&I2Cone);
   ina219_A2.begin (&I2Cone);
   ina219_A3.begin (&I2Cone);
   ina219_B1.begin (&I2Ctwo);
   ina219_B2.begin (&I2Ctwo);
}

//Adafruit_INA219 &ina219_A1, Adafruit_INA219 &ina219_A2, Adafruit_INA219 &ina219_A3, Adafruit_INA219 &ina219_B1, Adafruit_INA219 &ina219_B2
//void Ina::getData (Adafruit_INA219 &ina219_A1, Adafruit_INA219 &ina219_A2, Adafruit_INA219 &ina219_A3, Adafruit_INA219 &ina219_B1, Adafruit_INA219 &ina219_B2) {
//
//    this->stopped.store(false, std::memory_order_relaxed);
//
//    while(!this->stopped.load(std::memory_order_relaxed))
//    {
//        this->getVoltage(ina219_A1, ina219_A2, ina219_A3, ina219_B1, ina219_B2);
//        this->getCurrent(ina219_A1, ina219_A2, ina219_A3, ina219_B1, ina219_B2);
//    }
//
//    Serial.println();
//    Serial.println(this->shuntvoltageA1);
//}

//Adafruit_INA219 &ina219_A1, Adafruit_INA219 &ina219_A2, Adafruit_INA219 &ina219_A3, Adafruit_INA219 &ina219_B1, Adafruit_INA219 &ina219_B2
//void Ina::getCurrent (Adafruit_INA219 &ina219_A1, Adafruit_INA219 &ina219_A2, Adafruit_INA219 &ina219_A3, Adafruit_INA219 &ina219_B1, Adafruit_INA219 &ina219_B2) {
//    //std::lock_guard<std::mutex> guard(mtx);
//    this->currentA1 = ina219_A1.getCurrent_mA();
//    this->currentA2 = ina219_A2.getCurrent_mA();
//    this->currentA3 = ina219_A3.getCurrent_mA();
//    this->currentB1 = ina219_B1.getCurrent_mA();
//    this->currentB2 = ina219_B2.getCurrent_mA();
//
//    Serial.println();
//    Serial.println(currentA1);
//}
//
//void Ina::getVoltage (Adafruit_INA219 &ina219_A1, Adafruit_INA219 &ina219_A2, Adafruit_INA219 &ina219_A3, Adafruit_INA219 &ina219_B1, Adafruit_INA219 &ina219_B2) {
//    //std::lock_guard<std::mutex> guard(mtx);
//    this->shuntvoltageA1 = ina219_A1.getVoltage();
//    this->shuntvoltageA2 = ina219_A2.getVoltage();
//    this->shuntvoltageA3 = ina219_A3.getVoltage();
//    this->shuntvoltageB1 = ina219_B1.getVoltage();
//    this->shuntvoltageB2 = ina219_B2.getVoltage();
//
//    Serial.println();
//    Serial.println(currentA1);
//}

// void Ina::print (Adafruit_INA219 &ina219_A1, Adafruit_INA219 &ina219_A2, Adafruit_INA219 &ina219_A3, Adafruit_INA219 &ina219_B1, Adafruit_INA219 &ina219_B2) {
//     Serial.print (" Voltage A1 = "); Serial.print (shuntvoltageA1); Serial.print (" Current A1 = "); Serial.print (currentA1);
//     Serial.print (" Voltage A2 = "); Serial.print (shuntvoltageA2); Serial.print (" Current A2 = "); Serial.print (currentA2);
//     Serial.print (" Voltage A3 = "); Serial.print (shuntvoltageA3); Serial.print (" Current A3 = "); Serial.print (currentA3);

//     Serial.print (" Voltage B1 = "); Serial.print (shuntvoltageB1); Serial.print (" Current B1 = "); Serial.print (currentB1);
//     Serial.print (" Voltage B2 = "); Serial.print (shuntvoltageB2); Serial.print (" Current B2 = "); Serial.print (currentB2); 

//     Serial.println();
// }
