#include "ViivaPankki.h"

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

}
