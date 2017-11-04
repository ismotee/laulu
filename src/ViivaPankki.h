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
    
    Alue();
    Alue(float min_, float max_): aktiivinen(true),min(min_), max(max_){}
    bool onAlueella(float f) const;
    bool onAlueella_wrapped(float f, float wrapLimit = 255) const;
    float keskipiste() const;
    float range() const;
    void asetaKeskipisteenMukaan(float p, float r);
};


class viivaSortti{
public:
    static ofVec2f keskipiste;
    static bool onLahempana(const Viiva&, const Viiva&);
    static void jarjesta(std::vector<Viiva>&, ofVec2f P);
};


class viivojenInterpoloija{
public:
    Viiva kolmio[3];
    bool onkoPisteKolmionSisalla(ofVec2f P);
    bool etsiKolmio(std::vector<Viiva> viivat, ofVec2f P);
    ofColor interpoloiVari(ofVec2f P);
};



struct ViivaPankki : public viivojenInterpoloija{

public:

    //hakemisto josta viivat ladataan:
    ofDirectory arkisto;

    //haetaan arkistoon tiedostopolut ja ladataan viivat
    bool lataaArkisto(const std::string& polku);

    //mikä sarja on valittuna aineistosta:
    int sarja_i = 0;
    
    
    //valitaan käytössä oleva sarja ja ladataan viivat:
    bool valitseSarja(int i);
    void seuraavaSarja();
    void edellinenSarja();
    
    //kaikki ladatut viivat:
    vector<Viiva> viivat;
    
    //valintoja viivoista:
    vector<Viiva> valitutViivat;
    vector<Viiva> piirretytViivat;

    //viiva jota tällä hetkellä muokataan
    Viiva viivaNyt;

    // kalibrointi ja muokattava viiva pidetään erillään pankista.
    //muokattavaaa viivaa myös tarkastellaan kalibroinnin aikana
    
    //hae yksi viiva:
    Viiva& operator[](std::size_t idx)       { return viivat[idx]; }
    const Viiva& operator[](std::size_t idx) const { return viivat[idx]; }
    
    bool empty(){return viivat.empty();}
    long unsigned int size(){return viivat.size();}
    
    vector<float> samankaltaisuus;
    
    ViivaPankki() {}

    void aloitaUusiViivaNyt();
    void lisaaPiste(ofPoint paikka, float paine, VaiheetEnum vaihe);
    Viiva* etsiViiva();
    void asetaKohteeksi(int id);
    void valitseViivat(Alue savyAlue,Alue saturaatioAlue, Alue kirkkausAlue, Alue paksuusAlue, Alue sumeusAlue);
    void toglaaValinta(ofVec2f paksuusSumeus);
    void tyhjennaValinta();
    void laskeVari();
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
