#pragma once
#include "ofMain.h"
#include "Vaiheet.h"
#include "ViivaOhjain.h"
#include "Monitori.h"
#include "OscInterface.h"
#include "Viiva.h"
#include "Tilat.h"

struct Ohjain: public Tilat, public Vaiheet, public ViivaOhjain, public Monitori, public OscInterface {

    ofColor monitoriVari;
    
    void setup();
    void update();
    void updateMonitori();
    
    void keyPressed(int key);
    
    bool tallennetaan = false;

    VaiheetEnum improvisoi();

    VaiheetEnum kalibroi();
    
    VaiheetEnum keskeyta();

    VaiheetEnum kulje();

    VaiheetEnum lahestyKohdetta();

    VaiheetEnum laskeKohde();

    VaiheetEnum viimeistele();
    
    
    // Tilat funktiot
    
    void piirtaa();
    void soittaa();
    void rajaa();
    
    void updateOSC();
    
};
