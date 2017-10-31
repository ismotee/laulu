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
        return ofVec2f(paksuus, sumeus);
    }
};

struct OminaisuusSiivu {
    float arvo;
    float keskiarvo;
    float keskihajonta;
    float keskihajonnanKeskihajonta;
    float konvergenssi;
    
};

struct ViivanSiivu {
    ofPoint piste;
    ofColor vari;
    OminaisuusSiivu paine;
    OminaisuusSiivu paksuus;
    OminaisuusSiivu sumeus;
    VaiheetEnum vaihe;
};

struct ViivanOminaisuus {
    vector<float> arvot;
    vector<float> keskiarvot;
    vector<float> keskihajonnat;
    vector<float> keskihajonnanKeskihajonnat;
    vector<float> konvergenssit;

    unsigned int size() const { return arvot.size(); }
    unsigned int size() { return arvot.size(); }

    float& operator[](int x) {
        return arvot[x];
    }

    const float& operator[](int x) const {
        return arvot[x];
    }
    
    OminaisuusSiivu haeSiivu(int id);

    int tarkistaKoko(unsigned int otanta) const {
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
        keskihajonnanKeskihajonnat.push_back(keskihajonta(keskihajonnat, n));
    }

    void laskeUusinKonvergenssi() {
        if (keskiarvot.back() == 0)
            konvergenssit.push_back(0);
        else
            konvergenssit.push_back(1 - 5 * keskihajonnanKeskihajonnat.back() / keskiarvot.back());
    }

    void lisaaJaLaske(float uusi, int otanta) {
        arvot.push_back(uusi);
        laskeUusinKeskiarvo(otanta);
        laskeUusinKeskihajonta(otanta);
        laskeUusinKeskihajonnanKeskihajonta(otanta);
        laskeUusinKonvergenssi();
    }

    float back() { return arvot.back();}
    float back() const { return arvot.back();}

    bool empty() { return arvot.empty(); }
    bool empty() const { return arvot.empty(); }

    void resize(int i) {
        arvot.resize(i);
        keskiarvot.resize(i);
        keskihajonnat.resize(i);
        keskihajonnanKeskihajonnat.resize(i);
        konvergenssit.resize(i);
    }
    
    string toString() {
        if (empty())
            return "";
        return "arvo: " + std::to_string(arvot.back()) +
                "\nkeskiarvo: " + std::to_string(keskiarvot.back()) +
                "\nkeskihajonta: " + std::to_string(keskihajonnat.back()) +
                "\nkeskihajonnankeskihajonta: " + std::to_string(keskihajonnanKeskihajonnat.back()) +
                "\nkonvergenssi: " + std::to_string(konvergenssit.back()) + "\n";
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
    std::shared_ptr<Viiva> kohde;

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
    bool muodostaViiva( 
            const std::vector<ofPoint>& pisteet_,
            const std::vector<float>& paineet_,
            const std::vector<float>& paksuudet_,
            const std::vector<float>& sumeudet_,
            const std::vector<VaiheetEnum>& vaiheet_,
            const std::vector<ofColor>& varit_,
            std::string nimi_
    );
    
    std::string nimeaViiva(std::string format = "%F_%H-%M-%S");
    bool kulje();
    void lisaaPiste(ofPoint paikka, float paine, VaiheetEnum vaihe);
    void laskeUusimmat();
    void asetaKalibraatio();
    void asetaAlkuperainenKalibraatio();
    void tarkistaVaihe(VaiheetEnum vaihe);
    void tarkistaKalibraatio();
    void tarkistaImprovisaatio();
    void tarkistaLahestyKohdetta();
    ofVec2f paksuusSumeusVektori();
    ViivanSiivu haeSiivu(int id) {
        ViivanSiivu siivu;
        if(id < pisteet.size()) {
            siivu.piste = pisteet[id];
            siivu.vari = varit[id];
            siivu.paksuus = paksuus.haeSiivu(id);
            siivu.sumeus = sumeus.haeSiivu(id);
            siivu.paine = paine.haeSiivu(id);
            siivu.vaihe = vaiheet[id];
        }
        return siivu;
    }

    void asetaKohde(shared_ptr<Viiva> kohde_);

    void muokkaaVaria();
    void muokkaaVaria2(float maara);
    ofColor haeVari();
    void lahestyKohdetta();
    float muutoksenMaaraPolulla();
    void nollaaLaskurit();
    void tyhjennaOminaisuudet();
    
    void resize(int i);    

    ofxOscMessage makePisteAsOscMessage();
    ofxOscMessage makePaksuusAsOscMessage();
    ofxOscMessage makeSumeusAsOscMessage();

    unsigned int size() const { return pisteet.size(); }
    bool empty() const { return pisteet.empty(); }


protected:

};
