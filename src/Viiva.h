#pragma once
#include "ofMain.h"
#include "ofxOsc.h"
#include "tilastot.h"
#include "Vaiheet.h"

struct ViivanApufunktiot {
    ofVec3f variRGBtoVec3(ofColor col);
    ofColor variRGBfromVec3(ofVec3f vec);
    ofColor asetaHSLnMukaan(float lh, float ls, float ll);
    float getLightness(double s, double v);

};

struct Kalibraatio {
    float paksuus;
    float sumeus;
    float paksuusKa;
    float sumeusKa;
    float paksuusKh;
    float sumeusKh;
    float paksuusKhkh;
    float sumeusKhkh;
    float paksuusKonvergenssi;
    float sumeusKonvergenssi;
    
    ofColor vari;
    ofVec2f paksuusSumeusVektori() {
        return ofVec2f(paksuus,sumeus);
    }
};


struct ViivanOminaisuus {
    vector<float> arvot;
    vector<float> keskiarvot;
    vector<float> keskihajonnat;
    vector<float> keskihajonnanKeskihajonnat;
    vector<float> konvergenssit;

    unsigned int size() {
        return arvot.size();
    }

    float& operator[](int x) {
        return arvot[x];
    }

    int tarkistaKoko(unsigned int otanta) {
        int n = 0;
        if (otanta >= size())
            n = size();
        else if (otanta <= 0)
            return 0;
        else
            n = otanta;
        return n;
    }

    void laskeUusinKeskiarvo(int otanta) {
        int n = tarkistaKoko(otanta);
        keskiarvot.push_back(keskiarvo(arvot, n));
    }

    void laskeUusinKeskihajonta(int otanta) {
        int n = tarkistaKoko(otanta);
        keskihajonnat.push_back(keskihajonta(arvot, n));
    }

    void laskeUusinKeskihajonnanKeskihajonta(int otanta) {
        int n = tarkistaKoko(otanta);
        keskihajonnanKeskihajonnat.push_back(keskihajonta(keskihajonnat,n));
    }
    
    void laskeUusinKonvergenssi() {
        konvergenssit.push_back(1 - 5 * keskihajonnanKeskihajonnat.back() / keskiarvot.back());
    }
    
    void lisaaJaLaske(float uusi, int otanta) {
        arvot.push_back(uusi);
        laskeUusinKeskiarvo(otanta);
        laskeUusinKeskihajonta(otanta);
        laskeUusinKeskihajonnanKeskihajonta(otanta);
        laskeUusinKonvergenssi();
    }
    
    float back() {
        return arvot.back();
    }
    
    bool empty() {
        return arvot.empty();
    }
    
};

struct Viiva : public ViivanApufunktiot {
    //miten suurta osaa datasta käytetään tilastollisessa tarkastelussa:
    static const int OTANNAN_KOKO = 50;
    static const float MAX_KIIHTYVYYS;
    const char* versio = "1";

    std::string nimi;
    int improvisaatioLaskin;
    int lahestymisLaskuri;
    ViivanOminaisuus muutos;
    Viiva* kohde;
    
    bool kalibraatioValmis;
    bool improvisaatioValmis;
    bool lahestyKohdettaValmis;
    
    vector<ofPoint> pisteet;
    vector<ofColor> varit;
    vector<VaiheetEnum> vaiheet;
    vector<std::string> kohteet;
    ViivanOminaisuus paine;
    ViivanOminaisuus paksuus;
    ViivanOminaisuus sumeus;

    Kalibraatio kalibraatio;
    Kalibraatio alkuperainenKalibraatio;

    
    Viiva();
    
    std::string nimeaViiva(std::string format = "%F_%H-%M-%S");
    void lisaaPiste(ofPoint paikka, float paine, VaiheetEnum vaihe);
    void laskeUusimmat();
    void asetaKalibraatio();
    void asetaAlkuperainenKalibraatio();
    void tarkistaVaihe(VaiheetEnum vaihe);
    void tarkistaKalibraatio();
    void tarkistaImprovisaatio();
    void tarkistaLahestyKohdetta();
    ofVec2f paksuusSumeusVektori();

    void asetaKohde(Viiva* kohde_);
    
    void muokkaaVaria();
    void muokkaaVaria2(float maara);
    void lahestyKohdetta();
    float muutoksenMaaraPolulla();
    void nollaaLaskurit();

    ofxOscMessage makePisteAsOscMessage();
    ofxOscMessage makePaksuusAsOscMessage();
    ofxOscMessage makeSumeusAsOscMessage();

    unsigned int size(){return pisteet.size();}
    

protected:

};
