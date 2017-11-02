#pragma once
#include "ofMain.h"
#include "Viiva.h"
#include "tiedosto.h"
#include "Soitin.h"
#include "Vaiheet.h"

/* ViivaPankki
 * Varasto-luokka viivoille.
 * ViivaPankki hllinnoi viivojen tallennusta ja latausta sekä 
 * uuden viivan pankkiin lisäämistä. Luokka pyrkii poistamaan tarpeen 
 * säilyttää Viiva tyyppisiä olioita tämän luokna ulkopuolella. ViivaPankki -luokassa
 * ei tapahdu laskemista. kts. ViivaOhjain
 */

struct Alue {
    bool aktiivinen;
    float min;
    float max;
    
    Alue():aktiivinen(true),min(0),max(255){}
    Alue(float min_, float max_): aktiivinen(true),min(min_), max(max_){}
    bool onAlueella(float f) const;
    bool onAlueella_wrapped(float f, float wrapLimit = 255) const;
    float keskipiste() const;
    float range() const;
    void asetaKeskipisteenMukaan(float p, float r);
};


struct ViivaPankki {

    vector<Viiva> viivat;
    vector<Viiva> valitutViivat;
    vector<Viiva> piirretytViivat;
    // kalibrointi ja muokattava viiva pidetään erillään pankista.
    //muokattavaaa viivaa myös tarkastellaan kalibroinnin aikana
    
    
    Viiva& operator[](std::size_t idx)       { return viivat[idx]; }
    const Viiva& operator[](std::size_t idx) const { return viivat[idx]; }

    bool empty(){return viivat.empty();}
    long unsigned int size(){return viivat.size();}
    
    Viiva viivaNyt;
    vector<float> samankaltaisuus;
    
    ViivaPankki() {}

    void aloitaUusiViivaNyt();
    void lisaaPiste(ofPoint paikka, float paine, VaiheetEnum vaihe);
    Viiva* etsiViiva();
    void asetaKohteeksi(int id);
    void valitseViivat(Alue savyAlue,Alue saturaatioAlue, Alue kirkkausAlue, Alue paksuusAlue, Alue sumeusAlue);
    void toglaaValinta(ofVec2f paksuusSumeus);
    void valitseVari(ofVec2f paksuusSumeusKa);
    void viimeistelePiirto();
    void lataaPiirretytViivat(std::string polku);
    
    // lataus ja tallennus
    // tapahtuu aina hakemistokohtaisesti.
    // tiedosto nimetään automaattisesti ajan mukaan
    void tallennaHakemistoon(string polku);
    bool lataaHakemistosta(string polku);
        
private:
};
