#include "ViivaPankki.h"

float wrap(float f, float max) {
    if(f > max) {
        int n = (int)(f/max);
        f -= n * max;                
    }
    if(f < 0) {
        f *= -1;
        //poistetaan max:n monikerrat
        int n = (int)(f/max);
        f -= n*max;        
        f *= -1;//käännetään takaisin. Tämän jälkeen f on [-max ... 0]
        f += max; //nyt pitäisi olla [0...max]
    }
    return f;
}


bool Alue::onAlueella(float f) const {
    if (f < min) return false;
    else if (f > max) return false;
    else return true;
}


bool Alue::onAlueella_wrapped(float f, float wrapLimit) const{
    float r = this->range();
    float P = this->keskipiste();
    
    P = wrap(P, wrapLimit);
    f = wrap(f, wrapLimit);
    
    if(P >= r && P <= wrapLimit - r ) {
        if(f > P-r && f < P+r) return true;
        else return false;
    }
    else if (P < r) {
        if(f > P - r + wrapLimit || f < P+r) return true;
        else return false;
    }
    else if (P > wrapLimit - r) {
        if(f > P-r || f < P + r - wrapLimit) return true;
        else return false;
    }
}


float Alue::keskipiste() const{
    return range() + min;
}

float Alue::range() const{ //maksimietäisyys keskipisteestä
    return (max - min) / 2;
}

void Alue::asetaKeskipisteenMukaan(float p, float r) {
    min = p - r;
    max = p + r;
}


void ViivaPankki::aloitaUusiViivaNyt() {
    viivaNyt = Viiva();
}

void ViivaPankki::lisaaPiste(ofPoint paikka, float paine, VaiheetEnum vaihe) {
    
    if(vaihe == LaskeKohde) {
        
        shared_ptr<Viiva> ptr = make_shared<Viiva>(*etsiViiva());
        
        viivaNyt.asetaKohde(ptr);
    }
    viivaNyt.lisaaPiste(paikka,paine,vaihe);
}

Viiva* ViivaPankki::etsiViiva() {
    ofVec2f kalibrointiVec = viivaNyt.kalibraatio.paksuusSumeusVektori();
    ofVec2f muokattavaVec = viivaNyt.paksuusSumeusVektori();
    ofVec2f vertailuVec = (muokattavaVec - kalibrointiVec).getNormalized();
    float nearestValue = -100;
    int nearestId = -1;

    vector<float> samankaltaisuus(viivat.size(),0);
    
    // lasketaan samankaltaisuus
    for (int i = 0; i < viivat.size(); i++) {

        ofVec2f vec = viivat[i].paksuusSumeusVektori();
        ofVec2f suunta = (vec - kalibrointiVec).getNormalized();

        samankaltaisuus[i] += suunta.dot(vertailuVec);
    }


    for (int i = 0; i < samankaltaisuus.size(); i++) {
        if (samankaltaisuus[i] > nearestValue) {
            nearestValue = samankaltaisuus[i];
            nearestId = i;
        }
    }

#ifdef VIIVA_DEBUG
    cout << "etsiViiva, nearestValue: " << nearestValue << "\n";
    cout << "etsiViiva, nearestId: " << nearestId << "\n";
#endif

    return &viivat[nearestId];
}


void ViivaPankki::tallennaHakemistoon(string polku) {
    ofDirectory td(polku);
    if(!td.exists())
        td.createDirectory(polku);
    ofDirectory d("./");
    
    tiedosto::tallennaViiva(viivaNyt,d.getAbsolutePath() + "/" +polku);
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

void ViivaPankki::asetaKohteeksi(int id) {
    if(id < size())
        viivaNyt.asetaKohde(make_shared<Viiva>(viivat[id]));
}

void ViivaPankki::valitseViivat(Alue savyAlue, Alue saturaatioAlue, Alue kirkkausAlue, Alue paksuusAlue, Alue sumeusAlue) {
    
    valitutViivat.clear();
    
    for(int i = 0; i < viivat.size(); i ++) {
        Kalibraatio k = viivat[i].kalibraatio;
        
        if(!savyAlue.onAlueella_wrapped(k.vari.getHue(), 255) || !savyAlue.aktiivinen )
            continue;
        if(!saturaatioAlue.onAlueella(k.vari.getSaturation() ) || !saturaatioAlue.aktiivinen)
            continue;
        if(!kirkkausAlue.onAlueella(k.vari.getBrightness() ) || !kirkkausAlue.aktiivinen) 
            continue;
        if(!paksuusAlue.onAlueella(k.paksuus) || !paksuusAlue.aktiivinen)
            continue;
        if(!sumeusAlue.onAlueella(k.sumeus) || !sumeusAlue.aktiivinen)
            continue;
        
        //jos tähän päästiin, ollaan alueilla!
        valitutViivat.push_back(viivat[i]);
    }
}

// etsitään viivaa, joka on lähimpänä annettua Paksuus-Sumeus vektoria valituista Viivaoista. Jos löytyy, poistetaan. Jos ei, lisätään.
void ViivaPankki::toglaaValinta(ofVec2f paksuusSumeus) {

    if(viivat.empty())
        return;
    
    float lyhinEtaisyys = (viivat[0].kalibraatio.paksuusSumeusVektori()-paksuusSumeus).length();
    int lyhinId = 0;
    
    for(int i = 0; i < viivat.size();i++) {

        float etaisyys = (viivat[i].kalibraatio.paksuusSumeusVektori()-paksuusSumeus).length();

        if(etaisyys < lyhinEtaisyys) {
            lyhinEtaisyys = etaisyys;
            lyhinId = i;
        }
    }
    
    
    for(int i = 0; i < valitutViivat.size();i++) {
        
        // verrataan nimeen, koska nimi on aina uniikki. 
        if(viivat[lyhinId].nimi.compare(valitutViivat[i].nimi) == 0) {
            // poistetaan viiva ja lopetetaan metodi välittömästi
            valitutViivat.erase(valitutViivat.begin()+i);
            return;
        }
    }
    // jos ei löytynyt niin lisätään viiva
    valitutViivat.push_back(viivat[lyhinId]);
    
}

void ViivaPankki::viimeistelePiirto() {

}
