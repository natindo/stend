#include "lib.h"


//---PZEM---
InputAndOutput::InputAndOutput () {
    this->a = 0;
    this->v = 0;
    this->status = 0;
}

void InputAndOutput::setV (float v) {
    this->v = v;
}

float InputAndOutput::getV () {
    return this->v;
}

void InputAndOutput::setA (float a) {
    this->a = a;
}

float InputAndOutput::getA () {
    return this->a;
}


//---INPUT---
Input::Input () {
    this->dc_a = 0;
    this->ac_a = 0;
    this->dc_v = 0;
    this->ac_v = 0;
}

float Input::getDc_v () {
    return this->dc_v;
}

float Input::getDc_a () {
    return this->dc_a;
}

void Input::setAc_v (float a) {
    this->ac_v = a;
}

float Input::getAc_v () {
    return this->ac_v;
}

void Input::setAc_a (float a) {
    this->ac_a = a;
}

float Input::getAc_a () {
    return this->ac_a;
}

//--SERVER--
WorkWithServer::WorkWithServer () {
    authFlag = false;
    authTimer = 0;
    requestTimer = 0;
    wlConnectTimer = 0;
    pzemTimer = 0;
    adcTimer = 0;
    incomingPacket[1440] = {0};
    outgoingPacket[1440] = {0};
    std::string outgoingPacketString;
}

//--BATTERY--
Battery::Battery () {
    this->v = 0;
}

//--GROUP--
Group::Group () {
    this->a = 0;
    this->v = 0;
    this->status = 0;
}
