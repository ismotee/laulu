#include "OscInterface.h"
#include <iostream>
#include <fstream>
#include <sstream>


void OscInterface::setAddress(std::string ipAddress, int sender_port, int receiver_port) {
    sender.setup(ipAddress, sender_port);
    receiver.setup(receiver_port);
}

void OscInterface::sendFloat(std::string address, float value) {
    if (connection) {
        ofxOscMessage msg;
        msg.setAddress(address);
        msg.addFloatArg(value);
        sender.sendMessage(msg,false);
    }
}

void OscInterface::sendMessage(ofxOscMessage msg) {
    if(connection)
        sender.sendMessage(msg,false);
}


void OscInterface::connect() {
    connection = true;
}

void OscInterface::disconnect() {
    connection = false;
}

bool OscInterface::setAddressAndPortsFromFile(std::string path) {
    std::ifstream is(path);
    
    std::string address;
    int s_port;
    int r_port;
    
    if(!is.is_open()) return false;    
    
    std::stringstream ss;
    
    ss << is.rdbuf();
    ss >> address;
    ss >> s_port;
    ss >> r_port;
    
    setAddress(address,s_port,r_port);
    
    return true;
}

