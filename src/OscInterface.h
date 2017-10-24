#pragma once
#include "ofMain.h"
#include "ofxOsc.h"

struct OscInterface {
    
    bool connection;
    
    ofxOscReceiver receiver;
    ofxOscSender sender;

    void setAddress(std::string ipAddress, int sender_port = 9997, int receiver_port = 9998);
    bool setAddressAndPortsFromFile(std::string path);
    void sendFloat(std::string address, float value);
    void sendMessage(ofxOscMessage msg);
    void connect();
    void disconnect();

};
