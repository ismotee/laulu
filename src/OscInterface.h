#pragma once
#include "ofMain.h"
#include "ofxOsc.h"
#include "Vaiheet.h"
#include "Viiva.h"
#include "Tilat.h"


struct OscInterface {
    
    bool connection;
    bool setupDone;
    
    OscInterface():setupDone(false),connection(false) {}
    
    ofxOscReceiver receiver;
    ofxOscSender sender;

    void setAddress(std::string ipAddress, int sender_port = 9997, int receiver_port = 9998);
    bool setAddressAndPortsFromFile(std::string path);
    void sendFloat(std::string address, float value);
    void sendMessage(ofxOscMessage msg);
    void connect();
    void disconnect();

};

struct OscViiva: public OscInterface {
    void sendViiva(Viiva viiva, VaiheetEnum vaihe);
};