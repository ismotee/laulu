#pragma once
#include "ofMain.h"
#include "ViivaPankki.h"
#include "Viiva.h"

//#define VIIVA_DEBUG



struct ViivaOhjain {
    ViivaPankki pankki;
    string hakemisto;
    string tallennusHakemisto;
    vector<float> samankaltaisuus;
    vector<float> muutos;
    int lahestymisLaskuri;
    
    int nykyinenPolku, polkuLaskuri;
        
    void setup(string hakemisto_, string tallennusHakemisto_);
    void tallennaKalibrointi();
    
    // Tarkastaa kalibroinnin laadukkuuden. Jos kalibrointi valmis, palauttaa true, muuten false
    bool kulkeminen();
    bool kalibrointi(ofPoint paikka, float paine);
    bool improvisointi(ofPoint paikka, float paine);
    bool laskeKohdeVari();
    bool lahesty(ofPoint paikka, float paine);
    const Viiva& haeMuokattava() const;
    const Viiva& haeKalibrointi() const;
    
    
    void soita();
    void soitaTaakse();
    
    void edellinenViiva();
    void seuraavaViiva();

    int lukupaa;
    int soitettava_id;
    Viiva soitettava;
    
};
