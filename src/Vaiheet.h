#pragma once
#include "ofMain.h"
#include "Kyna.h"

enum VaiheetEnum {Kulje,Kalibroi,Improvisoi,LaskeKohde,LahestyKohdetta,Viimeistele,Keskeyta};


struct Vaiheet: public Kyna {

    VaiheetEnum vaiheetEnum;
    
    void setup();
    void update();
    
    virtual VaiheetEnum kulje() = 0;
    virtual VaiheetEnum kalibroi() = 0;
    virtual VaiheetEnum improvisoi() = 0;
    virtual VaiheetEnum laskeKohde() = 0;
    virtual VaiheetEnum lahestyKohdetta() = 0;
    virtual VaiheetEnum viimeistele() = 0;
    virtual VaiheetEnum keskeyta() = 0;
    void verbose();
    std::string toString();
};
