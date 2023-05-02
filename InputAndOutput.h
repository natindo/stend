#ifndef INPUTANDOUOTPUT_H
#define INPUTANDOUOTPUT_H
#include "lib.h"
//use only for pzem var
//v_ac = v   i_ac = a
//and for inputGroup
class InputAndOutput {
protected:
    float a;
    float v;
public:
    InputAndOutput();

    int status;
    void setV (float v);
    float getV ();
    void setA (float a);
    float getA ();
};

//inputs var
class Input : public InputAndOutput {
private:
    float dc_v;
    float dc_a;
    float ac_v;
    float ac_a;

public:
    Input();

    float getDc_v ();
    float getDc_a ();
    void setAc_v (float a);
    float getAc_v ();
    void setAc_a (float a);
    float getAc_a ();
};

//battery var
class Battery : public InputAndOutput {
public:
    Battery();
};

//measure var
class Group : public InputAndOutput {
public:
    Group();
};

//servers var
class WorkWithServer {
public:
    bool authFlag;
    unsigned long authTimer;
    unsigned long requestTimer;
    unsigned long wlConnectTimer;
    unsigned long pzemTimer;
    unsigned long adcTimer;
    char incomingPacket[1440];
    char outgoingPacket[1440];
    std::string outgoingPacketString;

    WorkWithServer();
};

#endif // INPUTANDOUOTPUT_H
