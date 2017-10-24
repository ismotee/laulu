#pragma once
#include "ofMain.h"
#include "Viiva.h"
#include "tiedosto.h"
#include "Soitin.h"

/* ViivaPankki
 * Varasto-luokka viivoille.
 * ViivaPankki hllinnoi viivojen tallennusta ja latausta sekä 
 * uuden viivan pankkiin lisäämistä. Luokka pyrkii poistamaan tarpeen 
 * säilyttää Viiva tyyppisiä olioita tämän luokna ulkopuolella. ViivaPankki -luokassa
 * ei tapahdu laskemista. kts. ViivaOhjain
 */


struct ViivaPankki: public Soitin {

    vector<Viiva> viivat;
    // kalibrointi ja muokattava viiva pidetään erillään pankista.
    //muokattavaaa viivaa myös tarkastellaan kalibroinnin aikana
    Viiva kalibrointi;
    Viiva muokattava;
    Viiva samankaltaisin;
    
    ViivaPankki() {Soitin::setup(&viivat);}
    // tällä hetkellä muokattava viiva lisätään viivat vectoriin
    void lisaaMuokattavaPankkiin();
    
    

    // nollausfunktiot kalibrointi ja muokattava Viivalle
    void aloitaUusiKalibrointi();
    void aloitaUusiMuokattava();
    
    // pisteen lisäysfunktiot kalibroinnille ja muokattavalle Viivalle
    void lisaaPisteKalibrointiin(ofPoint paikka, float paine);
    void lisaaPisteMuokattavaan(ofPoint paikka, float paine);
    
    
    // lataus ja tallennus
    // tapahtuu aina hakemistokohtaisesti.
    // tiedosto nimetään automaattisesti ajan mukaan
    void tallennaHakemistoon(string polku);
    bool lataaHakemistosta(string polku);
    
    void leikkaaMuokattava(unsigned int otanta);
    
    
    void teeKalibraatioMuutos();
};
