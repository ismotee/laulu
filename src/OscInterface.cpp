#include "OscInterface.h"
#include <iostream>
#include <fstream>
#include <sstream>



void OscInterface::setAddress(std::string ipAddress, int sender_port, int receiver_port) {
    if (!ipAddress.empty()) {
        sender.setup(ipAddress, sender_port);
        receiver.setup(receiver_port);
        setupDone = true;
    } else
        setupDone = false;
}

void OscInterface::sendFloat(std::string address, float value) {
    if (connection) {
        ofxOscMessage msg;
        msg.setAddress(address);
        msg.addFloatArg(value);
        sender.sendMessage(msg, false);
    }
}

void OscInterface::sendMessage(ofxOscMessage msg) {
    if (connection)
        sender.sendMessage(msg, false);
}

void OscInterface::connect() {
    if(setupDone)
        connection = true;
    else
        connection = false;
}

void OscInterface::disconnect() {
    connection = false;
}

bool OscInterface::setAddressAndPortsFromFile(std::string path) {
    std::ifstream is(path);

    std::string address;
    int s_port;
    int r_port;

    if (!is.is_open()) {
        cout << "OscSettings: ei voitu ladata\n";
        return false;
    }
    std::stringstream ss;

    ss << is.rdbuf();
    ss >> address;
    ss >> s_port;
    ss >> r_port;

    setAddress(address, s_port, r_port);

    return true;
}

void OscViiva::sendViiva(Viiva viiva) {
    if (connection && viiva.size()) {
        ofxOscMessage msg;
        if (viiva.size() > 2) {
            msg.setAddress("/Viiva/jatkuu");
        } else if (viiva.size() == 1) {
            msg.setAddress("/Viiva/alkaa");

        } else if (viiva.size() != 0 && viiva.vaiheet.back() == Keskeyta) {
            msg.setAddress("/Viiva/loppuu");
        }

        msg.addStringArg(viiva.nimi);
        msg.addStringArg(vaiheenNimi(viiva.vaiheet.back()));
        msg.addStringArg(std::to_string(viiva.pisteet.back().x));
        msg.addStringArg(std::to_string(viiva.pisteet.back().y));
        msg.addStringArg(to_string(viiva.paine.back()));
        msg.addStringArg(to_string(viiva.paksuus.back()));
        msg.addStringArg(to_string(viiva.sumeus.back()));

        sendMessage(msg);

    }
}
