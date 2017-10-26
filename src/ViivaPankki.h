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


struct ViivaPankki: public Soitin {

    vector<Viiva> viivat;
    // kalibrointi ja muokattava viiva pidetään erillään pankista.
    //muokattavaaa viivaa myös tarkastellaan kalibroinnin aikana
    
    Viiva viivaNyt;
    vector<float> samankaltaisuus;
    
    
    
    ViivaPankki() {}

    void aloitaUusiViivaNyt();
    void lisaaPiste(ofPoint paikka, float paine, VaiheetEnum vaihe);
    Viiva* etsiViiva();
    
    // lataus ja tallennus
    // tapahtuu aina hakemistokohtaisesti.
    // tiedosto nimetään automaattisesti ajan mukaan
    void tallennaHakemistoon(string polku);
    bool lataaHakemistosta(string polku);
        
};
