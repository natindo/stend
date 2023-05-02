#ifndef MESSAGEFROMTOSERVER_H
#define MESSAGEFROMTOSERVER_H
#include "lib.h"

class WorkServer {
public:
    
    WorkServer ();
    //~WorkServer();

    int countConnectToServer = 0;
    bool isConnectToServer = 0;

    void authToServer (WiFiClient client);
    void authFromServer (WiFiClient client);
    void fullAuth (WiFiClient client);
    void requestSendData (WiFiClient client, Group groupOut1, Group groupOut2, Group groupOut3, InputAndOutput groupInputSolar, InputAndOutput groupInputWind, InputAndOutput pzemClass, Battery battery);
    void sendData (WiFiClient client, Group groupOut1, Group groupOut2, Group groupOut3, InputAndOutput groupInputSolar, InputAndOutput groupInputWind, InputAndOutput pzemClass, Battery battery);
};

#endif // MESSAGEFROMTOSERVER_H
