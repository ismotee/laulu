#pragma once
#include "ofMain.h"
#include "Viiva.h"
#include "tiedosto.h"


struct Soitin {
    std::vector<Viiva>* viivat;
    Viiva nykyinen;
    int nykyinen_id;
    int lukupaa;
    bool uusiViiva;
    
    Soitin();
    
    void setup(std::vector<Viiva>* viivat_ptr);
    
    void lataa(std::string polku);
    void tallenna(std::string polku);
    
    void seuraavaViiva();
    void edellinenViiva();
    
    Viiva nykyinenViiva();
    
    Viiva soita();
    
    void katkaiseViivaKohdasta(int kohta);
    
};


