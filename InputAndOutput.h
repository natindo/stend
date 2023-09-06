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

#endif // INPUTANDOUOTPUT_H
