#include "lib.h"

WorkServer::WorkServer () {
    this->countConnectToServer = 0;
    this->isConnectToServer = 0;
}

void WorkServer::authToServer (WiFiClient client) {
    const char* authJson = "{\"type\": 1, \"data\": {\"login\": \"priority2030\",\"password\": \"12345\"}}\n";
    client.print(authJson);
}

void WorkServer::requestSendData (WiFiClient client, Group groupOut1, Group groupOut2, Group groupOut3, InputAndOutput groupInputSolar, InputAndOutput groupInputWind, InputAndOutput pzemClass, Battery battery) {
  int count = 0;
  while (client.available()) {
    char c = client.read();
    count++;
    if ((count == 9) && (c == '3')) {
      sendData(client, groupOut1, groupOut2, groupOut3, groupInputSolar, groupInputWind, pzemClass, battery);
    } else if ((count == 9) && (c != '3')) {
      break;
    }
  }
}

//dont know how it work
void WorkServer::authFromServer (WiFiClient client) {
    int count = 0;
    while (client.available()){
      char c = client.read();
      count++;
      if ((count == 56) && (c == '0')) {
        this->countConnectToServer = 0;
        this->isConnectToServer = 1;
        Serial.println("supermegakek");
        continue;
      }
      else if ((count == 57) && (c == 1))
      {
        Serial.println("errorServerNotUs");
        break;
      }
  }
}

void WorkServer::fullAuth (WiFiClient client) {
    int count = 0;
    bool err = false;
    while (this->isConnectToServer == 0) {
      if (count <= 5) {
        authToServer(client);
        delay(500);
        authFromServer(client);
        count++;
      } else if (this->isConnectToServer == 0 && count > 5) {
        //сделать работу в автономном режиме
        //autoWork();
        err = true;
        break;
      } else if (this->isConnectToServer == 1) {
        break;
      }
    }
}

// void WorkServer::sendData (WiFiClient client, Group groupOut1, Group groupOut2, Group groupOut3, InputAndOutput groupInputSolar, InputAndOutput groupInputWind, InputAndOutput pzemClass, Battery battery) {
//    const char* sendDataToServer = "{\"type\": 3, \"data\": {\"solar\": [%4.2f, %4.2f, %d], \"wind\": [%4.2f, %4.2f, %d], \"gen\": [%4.2f, %4.2f, %d], \"bat\": [%4.2f], \"1\": [%4.2f, %4.2f, %d], \"2\": [%4.2f, %4.2f, %d], \"3\": [%4.2f, %4.2f, %d]}}\n", 
//    groupInputSolar.getV(), groupInputSolar.getA(), groupInputSolar.status, groupInputWind.getV(), groupInputWind.getA(), groupInputWind.status, pzemClass.getV(), pzemClass.getA(), pzemClass.status, battery.getV(), groupOut1.getV(), groupOut1.getA(), groupOut1.status, groupOut2.getV(), groupOut2.getA(), groupOut2.status, groupOut3.getV(), groupOut3.getA(), groupOut3.status, 
//    ;
//    client.print(sendDataToServer);
// }
