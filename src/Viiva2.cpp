#include "Viiva2.h"
#include "tilastot.h"

ofColor asetaHSLnMukaan(float lh, float ls, float ll){
    float bh = lh;
    if(ll <=0.5) 
        ls *= ll;
    else
        ls*=1-ll;
    
    float bb = ll+ls;
    float bs = (2*ls) / (ll+ls);
    
    ofColor col = ofColor::white;
    col.setHsb(bh,bs*255,bb*255);
    
    return col;
}

float getLightness(double s, double v)
 {
      return ((2 - s) * v)/2;
 }



void Viiva2::lisaaPiste(ofPoint paikka, float paine) {
    //lisätään viivaan piste ja lasketaan sen ominaisuudet (sumeus, paksuus)

    //edellinen ja sitä edellinen piste: Jos ollaan lisäämässä ensimmäisiä pisteitä, käytetään default constructorin antamaa pistettä
    ViivanPiste viimePiste(paikka, paine);
    ViivanPiste toissaPiste(paikka, paine);    
    if (!pisteet.empty())
        viimePiste = pisteet.back();    
    if (pisteet.size() >= 2)
        toissaPiste = pisteet[pisteet.size() - 2];

    //lasketaan nopeus ja kiihtyvyys edellisten pisteiden sijainnin perusteella. Nopeus vektorina, kiihtyvyydestä riittää suuruus:
    ofVec2f uusiNopeus = paikka - viimePiste.sijainti;
    ofVec2f viimeNopeus = viimePiste.sijainti - toissaPiste.sijainti;

    float kiihtyvyys = (uusiNopeus - viimeNopeus).length();

    //paksuus on kiihtyvyys skaalattuna välille 0...1
    ViivanOminaisuus uusiPaksuus;
    uusiPaksuus.arvo = ofClamp(kiihtyvyys/MAX_KIIHTYVYYS, 0, 1);

    //lasketaan paksuuden keskiarvo ja keskihajonta
    std::vector<float> paksuudet;
    paksuudet = haeArvot(&paksuus, OTANNAN_KOKO);
    paksuudet.push_back(uusiPaksuus.arvo);
    uusiPaksuus.keskiarvo = keskiarvo(paksuudet);
    uusiPaksuus.keskihajonta = keskihajonta(paksuudet);

    //lasketaan paksuuden keskihajonnan keskihajonta
    std::vector<float> paksuudenHajonnat = haeKeskihajonnat(&paksuus, OTANNAN_KOKO);
    paksuudenHajonnat.push_back(uusiPaksuus.keskihajonta);
    uusiPaksuus.keskihajonnanKeskihajonta = keskihajonta(paksuudenHajonnat);

    //paksuuden konvergenssi eli ovatko viivan laadut vakiintuneet.
    //Konvergenssi on luokkaa -inf...1 Kun konvergenssi on luokkaa 0.6...1, voidaan laatujen tulkita vakiintuneen
    uusiPaksuus.konvergenssi = 1 - 5 * uusiPaksuus.keskihajonnanKeskihajonta / uusiPaksuus.keskiarvo;

    //sumeus on paineen vastakohta
    ViivanOminaisuus uusiSumeus;
    uusiSumeus.arvo = 1 - paine;

    //lasketaan sumeuden keskiarvo ja keskihajonta
    std::vector<float> sumeudet = haeArvot(&sumeus, OTANNAN_KOKO);
    sumeudet.push_back(uusiSumeus.arvo);
    uusiSumeus.keskiarvo = keskiarvo(sumeudet);
    uusiSumeus.keskihajonta = keskihajonta(sumeudet);

    //lasketaan sumeuden keskihajonnan keskihajonta
    std::vector<float> sumeudenHajonnat = haeKeskihajonnat(&sumeus, OTANNAN_KOKO);
    sumeudenHajonnat.push_back(uusiSumeus.keskihajonta);
    uusiSumeus.keskihajonnanKeskihajonta = keskihajonta(sumeudenHajonnat);

    //sumeuden konvergenssi -- huom. tämä ei riipu suuruudesta kuten paksuuden konvergenssi, sillä sumeuden mittakaava on aina sama 0...1
    uusiSumeus.konvergenssi = 1 - 5 * uusiSumeus.keskihajonnanKeskihajonta;

    //lisätään piste ja ominaisuudet viivaan
    pisteet.push_back(ViivanPiste(paikka, paine));
    paksuus.push_back(uusiPaksuus);
    sumeus.push_back(uusiSumeus);
    
    //korjataan vielä viivan alkupää, koska siihen jää muuten räjähtävä kiihtyvyys
    if(pisteet.size() == 3) {
        paksuus[0] = paksuus[2];
        paksuus[1] = paksuus[2];
        sumeus[0] = sumeus[2];
        sumeus[1] = sumeus[2];
    }
}



ViivanPiste Viiva2::haeViimeisinPiste() const {
    if (!pisteet.empty())
        return pisteet.back();
    //TODO: keksi parempi ratkaisu
    return ViivanPiste();
}

ViivanOminaisuus Viiva2::haeViimeisinPaksuus() const {
    if (!paksuus.empty())
        return paksuus.back();
    return ViivanOminaisuus();
}

ViivanOminaisuus Viiva2::haeViimeisinSumeus() const {
    if (!sumeus.empty())
        return sumeus.back();
    return ViivanOminaisuus();
}

ofVec2f Viiva2::paksuusSumeusVektori() {
    return ofVec2f(haeViimeisinPaksuus().keskiarvo,haeViimeisinSumeus().keskiarvo);
}



vector<float> Viiva2::haeArvot(const vector<ViivanOminaisuus>* const ominaisuus) const {
    //tarkistetaan että osoite on validi
    if (ominaisuus != &paksuus || ominaisuus != &sumeus)
        return vector<float>(0);

    vector<float> arvot;
    arvot.resize(ominaisuus->size(), 0);
    for (unsigned int i = 0; i < ominaisuus->size(); i++) {
        arvot[i] = (*ominaisuus)[i].arvo;
    }
    return arvot;
}

vector<float> Viiva2::haeArvot(const vector<ViivanOminaisuus>* const ominaisuus, unsigned int otanta) const {
    //tarkistetaan että osoite on validi
    if (ominaisuus != &paksuus && ominaisuus != &sumeus)
        return vector<float>(0);


    //lasketaan montako jäsentä oikeasti palautetaan = n    
    vector<float> arvot;
    int n;
    if (otanta >= ominaisuus->size())
        n = ominaisuus->size();
    else if (otanta <= 0)
        return vector<float>(0);
    else
        n = otanta;

    //kopioidaan arvot ominaisuudesta    
    arvot.resize(n);
    unsigned int start_i = ominaisuus->size() - n;
    for (unsigned int i = start_i; i < ominaisuus->size(); i++) {
        arvot[i-start_i] = (*ominaisuus)[i].arvo;
    }
    return arvot;
}

vector<float> Viiva2::haeKeskiarvot(const vector<ViivanOminaisuus>* const ominaisuus) const{
    vector<float> keskiarvot;
    keskiarvot.resize(ominaisuus->size(), 0);
    for (unsigned int i = 0; i < ominaisuus->size(); i++) {
        keskiarvot[i] = (*ominaisuus)[i].keskiarvo;
    }
    return keskiarvot;
}

vector<float> Viiva2::haeKeskiarvot(const vector<ViivanOminaisuus> * const ominaisuus, unsigned int otanta) const {
    //tarkistetaan että osoite on validi
    if (ominaisuus != &paksuus && ominaisuus != &sumeus)
        return vector<float>(0);


    //lasketaan montako jäsentä oikeasti palautetaan = n    
    vector<float> arvot;
    int n;
    if (otanta >= ominaisuus->size())
        n = ominaisuus->size();
    else if (otanta <= 0)
        return vector<float>(0);
    else
        n = otanta;

    //kopioidaan arvot ominaisuudesta    
    arvot.resize(n);
    unsigned int start_i = ominaisuus->size() - n;
    for (unsigned int i = start_i; i < ominaisuus->size(); i++) {
        arvot[i-start_i] = (*ominaisuus)[i].keskiarvo;
    }
    return arvot;

}


vector<float> Viiva2::haeKeskihajonnat(const vector<ViivanOminaisuus>* const ominaisuus) const {
    vector<float> keskihajonnat;
    keskihajonnat.resize(ominaisuus->size(), 0);
    for (unsigned int i = 0; i < ominaisuus->size(); i++) {
        keskihajonnat[i] = (*ominaisuus)[i].keskihajonta;
    }
    return keskihajonnat;
}

vector<float> Viiva2::haeKeskihajonnat(const vector<ViivanOminaisuus>* const ominaisuus, unsigned int otanta) const {
    //tarkistetaan että osoite on validi. Jos ei, palautetaan tyhjä vektori
    if (ominaisuus != &paksuus && ominaisuus != &sumeus)
        return vector<float>(0);

    //lasketaan montako jäsentä oikeasti palautetaan = n
    vector<float> keskihajonnat;
    int n;
    if (otanta >= ominaisuus->size())
        n = ominaisuus->size();
    else if (otanta <= 0)
        return vector<float>(0);
    else
        n = otanta;

    //kopioidaan arvot ominaisuudesta
    keskihajonnat.resize(n);
    unsigned int start_i = ominaisuus->size() - n;
    for (unsigned int i = start_i; i < ominaisuus->size(); i++)
        keskihajonnat[i-start_i] = (*ominaisuus)[i].keskihajonta;

    return keskihajonnat;
}



vector<float> Viiva2::haeKeskihajonnanKeskihajonnat(const vector<ViivanOminaisuus>* const ominaisuus) const {
    vector<float> keskihajonnanKeskihajonnat;
    keskihajonnanKeskihajonnat.resize(ominaisuus->size(), 0);
    for (unsigned int i = 0; i < ominaisuus->size(); i++) {
        keskihajonnanKeskihajonnat[i] = (*ominaisuus)[i].keskihajonnanKeskihajonta;
    }
    return keskihajonnanKeskihajonnat;

}

vector<float> Viiva2::haeKeskihajonnanKeskihajonnat(const vector<ViivanOminaisuus> * const ominaisuus, unsigned int otanta) const {
    //tarkistetaan että osoite on validi
    if (ominaisuus != &paksuus && ominaisuus != &sumeus)
        return vector<float>(0);


    //lasketaan montako jäsentä oikeasti palautetaan = n    
    vector<float> arvot;
    int n;
    if (otanta >= ominaisuus->size())
        n = ominaisuus->size();
    else if (otanta <= 0)
        return vector<float>(0);
    else
        n = otanta;

    //kopioidaan arvot ominaisuudesta    
    arvot.resize(n);
    unsigned int start_i = ominaisuus->size() - n;
    for (unsigned int i = start_i; i < ominaisuus->size(); i++) {
        arvot[i-start_i] = (*ominaisuus)[i].keskihajonnanKeskihajonta;
    }
    return arvot;

}


vector<float> Viiva2::haeKonvergenssit(const vector<ViivanOminaisuus>* const ominaisuus) const {
    vector<float> konvergenssit;
    konvergenssit.resize(ominaisuus->size(), 0);
    for (unsigned int i = 0; i < ominaisuus->size(); i++) {
        konvergenssit[i] = (*ominaisuus)[i].konvergenssi;
    }
    return konvergenssit;
}

vector<float> Viiva2::haeKonvergenssit(const vector<ViivanOminaisuus> * const ominaisuus, unsigned int otanta) const {
    //tarkistetaan että osoite on validi
    if (ominaisuus != &paksuus && ominaisuus != &sumeus)
        return vector<float>(0);


    //lasketaan montako jäsentä oikeasti palautetaan = n    
    vector<float> arvot;
    int n;
    if (otanta >= ominaisuus->size())
        n = ominaisuus->size();
    else if (otanta <= 0)
        return vector<float>(0);
    else
        n = otanta;

    //kopioidaan arvot ominaisuudesta    
    arvot.resize(n);
    unsigned int start_i = ominaisuus->size() - n;
    for (unsigned int i = start_i; i < ominaisuus->size(); i++) {
        arvot[i-start_i] = (*ominaisuus)[i].konvergenssi;
    }
    return arvot;

}


void Viiva2::muokkaaVaria(const ViivanOminaisuus& paksuusVahennys, const ViivanOminaisuus& sumeusVahennys) {
    
    float sumeusMuunnos = (haeViimeisinSumeus().keskiarvo - sumeusVahennys.keskiarvo) * 0.7;
    float paksuusMuunnos = (haeViimeisinPaksuus().keskiarvo - paksuusVahennys.keskiarvo) * 127 * 0.7;

    float lightness = getLightness(verrattava.getSaturation()/255,verrattava.getBrightness()/255)+sumeusMuunnos;
    varit.push_back(asetaHSLnMukaan(verrattava.getHue(),(verrattava.getSaturation()+paksuusMuunnos)/255,lightness));
    
}

void Viiva2::muokkaaVaria2(ofColor kohdeVari, float maara) {
    ofVec3f kohta = variRGBtoVec3(verrattava) + (variRGBtoVec3(kohdeVari) - variRGBtoVec3(verrattava)) * maara;
    varit.push_back(variRGBfromVec3(kohta));
    varit.back().r = ofClamp(varit.back().r,0,255);
    varit.back().g = ofClamp(varit.back().g,0,255);
    varit.back().b = ofClamp(varit.back().b,0,255);
}


ofVec3f Viiva2::variRGBtoVec3(ofColor col) {
    return ofVec3f(col.r,col.g,col.b);
}

ofColor Viiva2::variRGBfromVec3(ofVec3f vec) {
    return ofColor(ofClamp(vec.x, 0, 255), ofClamp(vec.y, 0, 255), ofClamp(vec.z, 0, 255));
}

ofxOscMessage Viiva2::makePisteAsOscMessage() {
    ofxOscMessage msg;
    msg.setAddress("/viiva/piste");
    msg.addFloatArg(haeViimeisinPiste().sijainti.x);
    msg.addFloatArg(haeViimeisinPiste().sijainti.y);
    msg.addFloatArg(haeViimeisinPiste().paine);
    return msg;
}

ofxOscMessage Viiva2::makePaksuusAsOscMessage() {
    ofxOscMessage msg;
    msg.setAddress("/viiva/paksuus");
    msg.addFloatArg(haeViimeisinPaksuus().arvo);
    msg.addFloatArg(haeViimeisinPaksuus().keskiarvo);
    msg.addFloatArg(haeViimeisinPaksuus().keskihajonta);
    msg.addFloatArg(haeViimeisinPaksuus().konvergenssi);
    return msg;
}

ofxOscMessage Viiva2::makeSumeusAsOscMessage() {
    ofxOscMessage msg;
    msg.setAddress("/viiva/sumeus");
    msg.addFloatArg(haeViimeisinSumeus().arvo);
    msg.addFloatArg(haeViimeisinSumeus().keskiarvo);
    msg.addFloatArg(haeViimeisinSumeus().keskihajonta);
    msg.addFloatArg(haeViimeisinSumeus().konvergenssi);
    return msg;
}

void Viiva2::asetaVerrattava () {
    verrattava = varit.back();
}