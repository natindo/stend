#include "esp32-hal-gpio.h"
#include "Relay.h"
#include <string>
#define _GLIBCXX_USE_CXX11_ABI 0

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

std::string Relay::relayHandler(int relayNum, char *relayCommand) {
  std::string tmp;
  if (relayNum < 0 || relayNum > 4) {
    tmp = "Wrong relay number";
    // strcpy(message, "Wrong relay number");
    // _udp.print("Wrong relay number");
    return tmp;
  }

  if (strncmp(relayCommand, "STATUS", 6) == 0) {
    tmp = getStatus(relayNum);
  } else if (strncmp(relayCommand, "ON", 2) == 0) {
    tmp = turnON(relayNum);
  } else if (strncmp(relayCommand, "OFF", 3) == 0) {
    tmp = turnOFF(relayNum);
  } else if (strncmp(relayCommand, "SWITCH", 6) == 0) {
    tmp = switchState(relayNum);
  } else {
    tmp = "No such relay command";
    // strcpy(message, "No such relay command");
    // _udp.print("No such relay command");
  }
  // Serial.println(tmp);
  return tmp;
}

std::string Relay::getStatus(int relayNum) {
  std::string tmp;
  // int intRelayNum = relayNum - '0';
  //    Serial.println(intRelayNum);
  //    Serial.println((int)(relayNum - '0'));
  switch (relayNum) {
    case 1:
      tmp = (digitalRead(_relay1) == LOW) ? "RELAY 1 ON" : "RELAY 1 OFF";
      break;
    case 2:
      tmp = (digitalRead(_relay2) == LOW) ? "RELAY 2 ON" : "RELAY 2 OFF";
      break;
    case 3:
      tmp = (digitalRead(_relay3) == LOW) ? "RELAY 3 ON" : "RELAY 3 OFF";
      break;
    case 4:
      tmp = (digitalRead(_relay4) == LOW) ? "RELAY 4 ON" : "RELAY 4 OFF";
      break;
    default:
      tmp = "Error in getStatus function";
  }
  return tmp;
}

std::string Relay::turnON(int relayNum) {
  std::string tmp;
  switch (relayNum) {
    case 1:
      digitalWrite(_relay1, LOW);
      tmp = "RELAY 1 ON";
      // _udp.print("RELAY 1 ON");
      break;
    case 2:
      digitalWrite(_relay2, LOW);
      tmp = "RELAY 2 ON";
      // _udp.print("RELAY 2 ON");
      break;
    case 3:
      digitalWrite(_relay3, LOW);
      tmp = "RELAY 3 ON";
      // _udp.print("RELAY 3 ON");
      break;
    case 4:
      digitalWrite(_relay4, LOW);
      tmp = "RELAY 4 ON";
      // _udp.print("RELAY 4 ON");
      break;
  }
  // strcpy(message, tmp);
  return tmp;
}

std::string Relay::turnOFF(int relayNum) {
  std::string tmp;
  switch (relayNum) {
    case 1:
      digitalWrite(_relay1, HIGH);
      tmp = "RELAY 1 OFF";
      // strcpy(tmp, "RELAY 1 OFF");
      // _udp.print("RELAY 1 OFF");
      break;
    case 2:
      digitalWrite(_relay2, HIGH);
      tmp = "RELAY 2 OFF";
      // strcpy(tmp, "RELAY 2 OFF");
      // _udp.print("RELAY 2 OFF");
      break;
    case 3:
      digitalWrite(_relay3, HIGH);
      tmp = "RELAY 3 OFF";
      // strcpy(tmp, "RELAY 3 OFF");
      // _udp.print("RELAY 3 OFF");
      break;
    case 4:
      digitalWrite(_relay4, HIGH);
      tmp = "RELAY 4 OFF";
      // strcpy(tmp, "RELAY 4 OFF");
      // _udp.print("RELAY 4 OFF");
      break;
  }
  // strcpy(message, tmp);
  return tmp;
}

std::string Relay::switchState(int relayNum) {
  std::string tmp;
  switch (relayNum) {
    case 1:
      digitalWrite(_relay1, !digitalRead(_relay1));
      tmp = (digitalRead(_relay1) == LOW) ? "RELAY 1 ON" : "RELAY 1 OFF";
      break;
    case 2:
      digitalWrite(_relay2, !digitalRead(_relay2));
      tmp = (digitalRead(_relay2) == LOW) ? "RELAY 2 ON" : "RELAY 2 OFF";
      break;
    case 3:
      digitalWrite(_relay3, !digitalRead(_relay3));
      tmp = (digitalRead(_relay3) == LOW) ? "RELAY 3 ON" : "RELAY 3 OFF";
      break;
    case 4:
      digitalWrite(_relay4, !digitalRead(_relay4));
      tmp = (digitalRead(_relay4) == LOW) ? "RELAY 4 ON" : "RELAY 4 OFF";
      break;
  }
  // strcpy(message, tmp);
  // _udp.print(tmp);
  return tmp;
}
