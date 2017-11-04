#include "OscInterface.h"
#include <iostream>
#include <fstream>
#include <sstream>

OscInterface::OscInterface() {

}


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

void OscViiva::sendViiva(Viiva viiva, VaiheetEnum vaihe) {
    
    
    if (connection && viiva.size()) {
        ofxOscMessage msg;

        
        
        
        if (viiva.size() == 1) {
            msg.setAddress("/viiva/alkaa");
            msg.addFloatArg(1);
            sendMessage(msg);
        } else if (vaihe == Keskeyta) {
            msg.setAddress("/viiva/loppuu");
            msg.addFloatArg(1);
            sendMessage(msg);
        }
        
        
        msg = ofxOscMessage();
        msg.setAddress("/viiva/nimi");
        msg.addStringArg(viiva.nimi);
        sendMessage(msg);

        msg = ofxOscMessage();
        msg.setAddress("/viiva/vaihe");
        msg.addStringArg(vaiheenNimi(viiva.vaiheet.back()));
        sendMessage(msg);
        
        msg = ofxOscMessage();
        msg.setAddress("/viiva/piste");
        msg.addFloatArg(viiva.pisteet.back().x);
        msg.addFloatArg(viiva.pisteet.back().y);
        sendMessage(msg);
        
        msg = ofxOscMessage();
        msg.setAddress("/viiva/paine");
        msg.addFloatArg(viiva.paine.back());
        msg.addFloatArg(viiva.paine.keskiarvot.back());
        msg.addFloatArg(viiva.paine.keskihajonnat.back());
        msg.addFloatArg(viiva.paine.konvergenssit.back());
        sendMessage(msg);
        
        
        msg = ofxOscMessage();
        msg.setAddress("/viiva/sumeus");
        msg.addFloatArg(viiva.sumeus.back());
        msg.addFloatArg(viiva.sumeus.keskiarvot.back());
        msg.addFloatArg(viiva.sumeus.keskihajonnat.back());
        msg.addFloatArg(viiva.sumeus.konvergenssit.back());
        sendMessage(msg);
        
        msg = ofxOscMessage();
        msg.setAddress("/viiva/paksuus");
        msg.addFloatArg(viiva.paksuus.back());
        msg.addFloatArg(viiva.paksuus.keskiarvot.back());
        msg.addFloatArg(viiva.paksuus.keskihajonnat.back());
        msg.addFloatArg(viiva.paksuus.konvergenssit.back());
        sendMessage(msg);

        msg = ofxOscMessage();
        msg.setAddress("/viiva/vari");
        msg.addFloatArg(viiva.varit.back().getHue());
        msg.addFloatArg(viiva.varit.back().getSaturation());
        msg.addFloatArg(viiva.varit.back().getBrightness());
        sendMessage(msg);

    }
}

void OscViiva::sendPlaybackPlay(Viiva viiva, int indeksi) {

    if(connection && viiva.size()) {
        ofxOscMessage msg;
        msg.setAddress("/playback/play");
        msg.addFloatArg(indeksi+1);
        
        sendMessage(msg);
        
    }
}

void OscViiva::sendPlaybackStop() {
    if(connection) {
        ofxOscMessage msg;
        msg.setAddress("/playback/stop");
        msg.addFloatArg(0);
        
        sendMessage(msg);
        
    }

}
