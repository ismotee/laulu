#pragma once
#include "ofMain.h"
#include "ofxOsc.h"
#include "Viiva.h"

struct Viiva2 {
    //miten suurta osaa datasta käytetään tilastollisessa tarkastelussa:
    static const int OTANNAN_KOKO;
    static const float MAX_KIIHTYVYYS;
    
    vector<ViivanPiste> pisteet;
    vector<ViivanOminaisuus> paksuus;
    vector<ViivanOminaisuus> sumeus;
    vector<ofColor> varit;
    ofColor verrattava;

    void lisaaPiste(ofPoint paikka, float paine);
    
    ViivanPiste haeViimeisinPiste() const;
    ViivanOminaisuus haeViimeisinPaksuus() const;
    ViivanOminaisuus haeViimeisinSumeus() const;
    ofVec2f paksuusSumeusVektori();
    ofVec3f variRGBtoVec3(ofColor col);
    ofColor variRGBfromVec3(ofVec3f vec);
    
    vector<float> haeArvot(const vector<ViivanOminaisuus>* const ominaisuus) const;
    vector<float> haeArvot(const vector<ViivanOminaisuus>* const ominaisuus, unsigned int otanta) const;
    
    vector<float> haeKeskiarvot(const vector<ViivanOminaisuus>* const ominaisuus) const;
    vector<float> haeKeskiarvot(const vector<ViivanOminaisuus>* const ominaisuus, unsigned int otanta) const;
    vector<float> haeKeskihajonnat(const vector<ViivanOminaisuus>* const ominaisuus) const;
    vector<float> haeKeskihajonnat(const vector<ViivanOminaisuus>* const ominaisuus, unsigned int otanta) const;
    vector<float> haeKeskihajonnanKeskihajonnat(const vector<ViivanOminaisuus>* const ominaisuus) const;
    vector<float> haeKeskihajonnanKeskihajonnat(const vector<ViivanOminaisuus>* const ominaisuus, unsigned int otanta) const;
    vector<float> haeKonvergenssit(const vector<ViivanOminaisuus>* const ominaisuus) const;
    vector<float> haeKonvergenssit(const vector<ViivanOminaisuus>* const ominaisuus, unsigned int otanta) const;
    
    void muokkaaVaria(const ViivanOminaisuus& paksuus, const ViivanOminaisuus& sumeus);
    void muokkaaVaria2(ofColor kohdeVari, float maara);
    void asetaVerrattava();
    
    
    ofxOscMessage makePisteAsOscMessage();
    ofxOscMessage makePaksuusAsOscMessage();
    ofxOscMessage makeSumeusAsOscMessage();
    
    
    
protected:
    
};
