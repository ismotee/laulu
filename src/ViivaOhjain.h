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
    bool laskeKohde(ofPoint paikka, float paine);
    bool lahesty(ofPoint paikka, float paine);
    bool viimeistele();
    const Viiva& haeMuokattava() const;
    const Viiva& haeKalibrointi() const;
    
    
    void soita();
    void meneAlkuun();
    
    void edellinenViiva();
    void seuraavaViiva();

    void edellinenViivaPlayback();
    void seuraavaViivaPlayback();

    int lukupaa;
    int lukupaaPlayback;
    int soitettava_id;
    int soitettavaPlayback_id;
    Viiva soitettava;
    
    bool soitaPlayback();
    void playbackAlkuun();
    
};
