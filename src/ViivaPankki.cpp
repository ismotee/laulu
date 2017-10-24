#include "ViivaPankki.h"

void ViivaPankki::lisaaMuokattavaPankkiin() {
    viivat.push_back(muokattava);   
}

void ViivaPankki::aloitaUusiMuokattava() {    
    muokattava = Viiva();
}

void ViivaPankki::aloitaUusiKalibrointi() {
    kalibrointi = Viiva();
}

void ViivaPankki::lisaaPisteKalibrointiin(ofPoint paikka, float paine) {
    kalibrointi.lisaaPiste(paikka,paine);
}

void ViivaPankki::lisaaPisteMuokattavaan(ofPoint paikka, float paine) {
    muokattava.lisaaPiste(paikka,paine);
}

void ViivaPankki::tallennaHakemistoon(string polku) {
    ofDirectory td(polku);
    if(!td.exists())
        td.createDirectory(polku);
    ofDirectory d("./");
    
    tiedosto::tallennaViiva(muokattava,d.getAbsolutePath() + "/" +polku);
}

bool ViivaPankki::lataaHakemistosta(string polku) {
    ofDirectory dir(polku);
    if(!dir.exists()) {
        cout << "yritettiin ladata " << dir.getAbsolutePath() << "\n";
        return false;
    }   
        cout << "ladataan " << dir.getAbsolutePath() << "\n";
    
    dir.listDir();
    
        
    for(int i = 0; i < dir.size();i++)
        viivat.push_back(tiedosto::lataaViiva(dir[i].getAbsolutePath()));
    
}

void ViivaPankki::teeKalibraatioMuutos() {
    muokattava.muokkaaVaria(kalibrointi.haeViimeisinPaksuus(),kalibrointi.haeViimeisinSumeus());
}

void ViivaPankki::leikkaaMuokattava(unsigned int otanta) {
    if(otanta>muokattava.pisteet.size())
        return;
    
    
    muokattava.pisteet.erase(muokattava.pisteet.begin(),muokattava.pisteet.end()-otanta);
    muokattava.paksuus.erase(muokattava.paksuus.begin(),muokattava.paksuus.end()-otanta);
    muokattava.sumeus.erase(muokattava.sumeus.begin(),muokattava.sumeus.end()-otanta);
    
}
