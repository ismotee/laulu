#include "Viiva.h"
#include "tilastot.h"

const float Viiva::MAX_KIIHTYVYYS = 1.5;

ofColor ViivanApufunktiot::asetaHSLnMukaan(float lh, float ls, float ll) {
    float bh = lh;
    if (ll <= 0.5)
        ls *= ll;
    else
        ls *= 1 - ll;

    float bb = ll + ls;
    float bs = (2 * ls) / (ll + ls);

    ofColor col = ofColor::white;
    col.setHsb(bh, bs * 255, bb * 255);

    return col;
}

float ViivanApufunktiot::getLightness(double s, double v) {
    return ((2 - s) * v) / 2;
}

ofVec2f Viiva::paksuusSumeusVektori() {
    return ofVec2f(paksuus.back(), sumeus.back());
}

ofVec3f ViivanApufunktiot::variRGBtoVec3(ofColor col) {
    return ofVec3f(col.r, col.g, col.b);
}

ofColor ViivanApufunktiot::variRGBfromVec3(ofVec3f vec) {
    return ofColor(ofClamp(vec.x, 0, 255), ofClamp(vec.y, 0, 255), ofClamp(vec.z, 0, 255));
}



//-----------------------------------------------------------------------------------------------------

std::string Viiva::nimeaViiva(std::string format) {
    //aikamuoto on vvvv-kk-pp_hh-mm-ss
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, format.c_str(), timeinfo);
    puts(buffer);
    return std::string(buffer);
}

Viiva::Viiva() : improvisaatioLaskin(0), kalibraatioValmis(false),improvisaatioValmis(false),kohde(NULL) {
    nimi = "viiva_" + nimeaViiva();
}

bool Viiva::muodostaViiva( 
        const std::vector<ofPoint>& pisteet_, 
        const std::vector<float>& paineet_, 
        const std::vector<float>& paksuudet_, 
        const std::vector<float>& sumeudet_, 
        const std::vector<VaiheetEnum>& vaiheet_, 
        const std::vector<ofColor>& varit_,
        std::string nimi_
) {
    tyhjennaOminaisuudet();
    int koko = pisteet_.size();
    if( koko != paineet_.size()
        || koko != paksuudet_.size()
        || koko != sumeudet_.size() 
        || koko != vaiheet_.size() 
        || koko != varit_.size() 
    ) {
        cout << "muodosta viiva: vektorien koko ei täsmää: " << pisteet_.size() << " " << paineet_.size() << " " << paksuudet_.size() << " " << sumeudet_.size() << " " << vaiheet_.size() << " " << varit_.size() << "\n";
        return false; //vektorit eivät ole samankokoisia!
    }
    pisteet = pisteet_;
    vaiheet = vaiheet_;
    varit = varit_;
    nimi = nimi_;
    
    for(int i=0; i<koko; i++) {
        paine.lisaaJaLaske(paineet_[i], OTANNAN_KOKO);
        paksuus.lisaaJaLaske(paksuudet_[i], OTANNAN_KOKO);
        sumeus.lisaaJaLaske(sumeudet_[i], OTANNAN_KOKO);
    } 
    return true;
}

void Viiva::lisaaPiste(ofPoint paikka, float paine_, VaiheetEnum vaihe) {

    if (!vaiheet.empty())
        tarkistaVaihe(vaihe);


    vaiheet.push_back(vaihe);
    //lisaa uusin paine ja laske keskiarvot ja muut
    paine.lisaaJaLaske(paine_, OTANNAN_KOKO);

    ofVec2f uusiNopeus;
    ofVec2f viimeNopeus;

    //lasketaan nopeus ja kiihtyvyys edellisten pisteiden sijainnin perusteella. Nopeus vektorina, kiihtyvyydestä riittää suuruus:

    if (pisteet.size() >= 1)
        uusiNopeus = paikka - pisteet.back();
    if (pisteet.size() >= 2)
        viimeNopeus = pisteet.back() - pisteet[pisteet.size() - 2];

    float kiihtyvyys = (uusiNopeus - viimeNopeus).length();

    //paksuus on kiihtyvyys skaalattuna välille 0...1
    float uusiPaksuus = ofClamp(kiihtyvyys / MAX_KIIHTYVYYS, 0, 1);
    paksuus.lisaaJaLaske(uusiPaksuus, OTANNAN_KOKO);


    //sumeus on paineen vastakohta
    float uusiSumeus = 1 - paine.back();
    sumeus.lisaaJaLaske(uusiSumeus, OTANNAN_KOKO);

    //lisätään piste ja ominaisuudet viivaan
    pisteet.push_back(paikka);

    //korjataan vielä viivan alkupää, koska siihen jää muuten räjähtävä kiihtyvyys
    if (pisteet.size() == 3) {
        paksuus[0] = paksuus[2];
        paksuus[1] = paksuus[2];
        sumeus[0] = sumeus[2];
        sumeus[1] = sumeus[2];
    }


    // cout << pisteet.size() << " " << paine.size() << " " << sumeus.size() << " " << paksuus.size() << " " << varit.size() << " " << vaiheet.size() << "\n";
}

void Viiva::tarkistaVaihe(VaiheetEnum vaihe) {


    if (vaihe != vaiheet.back())
        nollaaLaskurit();

    // Kalibraatio
    if (vaihe == Kalibroi)
        tarkistaKalibraatio();
    // Improvisaatio
    if (vaiheet.back() == Kalibroi && vaihe == Improvisoi) {
        asetaKalibraatio();
        asetaAlkuperainenKalibraatio();
        tyhjennaOminaisuudet();
        varit.push_back(kalibraatio.vari);
        
    } else if (vaihe == Improvisoi) {
        tarkistaImprovisaatio();
    }

    //LahestyKohdetta
    if (vaihe == LahestyKohdetta) {
        lahestyKohdetta();
        tarkistaLahestyKohdetta();
    }

    if (vaihe != LahestyKohdetta && vaiheet.back() == LahestyKohdetta) {
        muutos = ViivanOminaisuus();
        kohde.reset();
    }

}

void Viiva::tarkistaKalibraatio() {
    float konvergenssi;
    if (paksuus.konvergenssit.back() == 0)
        konvergenssi = sumeus.konvergenssit.back();
    else if (sumeus.konvergenssit.back() == 0)
        konvergenssi = paksuus.konvergenssit.back();
    else
        konvergenssi = paksuus.konvergenssit.back() * sumeus.konvergenssit.back();


    if (konvergenssi > 0.6)
        kalibraatioValmis = true;
    else
        kalibraatioValmis = false;
}

void Viiva::tarkistaImprovisaatio() {

    if ((paksuusSumeusVektori() - kalibraatio.paksuusSumeusVektori()).length() > 0.02) {
        improvisaatioLaskin++;
    } else {
        improvisaatioLaskin = 0;
    }
    if (improvisaatioLaskin > 10) {
        improvisaatioValmis = true;
    }

    improvisaatioValmis = false;
}

void Viiva::tarkistaLahestyKohdetta() {
    lahestymisLaskuri++;


    int start_i = muutos.size() - 150;
    if (start_i < 0) start_i = 0;

    vector<float> arvot;
    arvot.resize(muutos.size() - start_i, 0);

    //cout << "start_i" << start_i << "\n";

    for (int i = start_i; i < muutos.size(); i++) {
        arvot[i - start_i] = muutos[i];
    }

    float muutoksenKeskihajonta = keskihajonta(arvot);
    cout << "muutoksen keskihajonta: " << muutoksenKeskihajonta << "\n";

    if (muutoksenKeskihajonta < 0.003 && lahestymisLaskuri > 150)
        lahestyKohdettaValmis = true;


    lahestymisLaskuri = false;
}

void Viiva::lahestyKohdetta() {
    muutos.lisaaJaLaske(muutoksenMaaraPolulla(), OTANNAN_KOKO);

    muokkaaVaria2(muutos.back());

}

void Viiva::asetaKalibraatio() {
    kalibraatio.vari = haeVari();

    kalibraatio.paksuus = paksuus.back();
    kalibraatio.paksuusKa = paksuus.keskiarvot.back();
    kalibraatio.paksuusKh = paksuus.keskihajonnat.back();
    kalibraatio.paksuusKhkh = paksuus.keskihajonnanKeskihajonnat.back();
    kalibraatio.paksuusKonvergenssi = paksuus.konvergenssit.back();

    kalibraatio.sumeus = sumeus.back();
    kalibraatio.sumeusKa = sumeus.keskiarvot.back();
    kalibraatio.sumeusKh = sumeus.keskihajonnat.back();
    kalibraatio.sumeusKhkh = sumeus.keskihajonnanKeskihajonnat.back();
    kalibraatio.sumeusKonvergenssi = sumeus.konvergenssit.back();
}

void Viiva::asetaAlkuperainenKalibraatio() {
    alkuperainenKalibraatio = kalibraatio;
}

void Viiva::muokkaaVaria() {

    float sumeusMuunnos = (sumeus.keskiarvot.back() - kalibraatio.sumeusKa) * 0.7;
    float paksuusMuunnos = (paksuus.keskiarvot.back() - kalibraatio.sumeusKa) * 127 * 0.7;

    float lightness = getLightness(kalibraatio.vari.getSaturation() / 255, kalibraatio.vari.getBrightness() / 255) + sumeusMuunnos;
    varit.push_back(asetaHSLnMukaan(kalibraatio.vari.getHue(), (kalibraatio.vari.getSaturation() + paksuusMuunnos) / 255, lightness));

}

float Viiva::muutoksenMaaraPolulla() {
    //lasketaan käyttäjän improvisoinninaikaisen eleen muutoksen projektio vektorilla, joka osoittaa lähtöpisteestä päämäärään, PS-koordinaatistossa.
    //skaalataan niin, että päämäärän kohdalla projektio on 1 ja lähtöpisteessä 0
    if (!kohde)
        return 0;

    //eli muokattavan projektio samankaltaisimmalla
    ofVec2f m = paksuusSumeusVektori();
    ofVec2f s = kohde->paksuusSumeusVektori();
    ofVec2f k = kalibraatio.paksuusSumeusVektori();

    float projektio = (m - k).dot((s - k).getNormalized()) / (s - k).length();

    return ofClamp(projektio, -0.5, 1.2);

}

void Viiva::muokkaaVaria2(float maara) {


    ofVec3f kohta = variRGBtoVec3(kalibraatio.vari) + (variRGBtoVec3(kohde->kalibraatio.vari) - variRGBtoVec3(kalibraatio.vari)) * maara;

    varit.push_back(variRGBfromVec3(kohta));
    varit.back().r = ofClamp(varit.back().r, 0, 255);
    varit.back().g = ofClamp(varit.back().g, 0, 255);
    varit.back().b = ofClamp(varit.back().b, 0, 255);
}

ofColor Viiva::haeVari() {
    if (varit.empty())
        return ofColor();
    return varit.back();
}

ofxOscMessage Viiva::makePisteAsOscMessage() {
    ofxOscMessage msg;
    msg.setAddress("/viiva/piste");
    msg.addFloatArg(pisteet.back().x);
    msg.addFloatArg(pisteet.back().y);
    msg.addFloatArg(paine.back());
    return msg;
}

ofxOscMessage Viiva::makePaksuusAsOscMessage() {
    ofxOscMessage msg;
    msg.setAddress("/viiva/paksuus");
    msg.addFloatArg(paksuus.back());
    msg.addFloatArg(paksuus.keskiarvot.back());
    msg.addFloatArg(paksuus.keskihajonnat.back());
    msg.addFloatArg(paksuus.konvergenssit.back());
    return msg;
}

ofxOscMessage Viiva::makeSumeusAsOscMessage() {
    ofxOscMessage msg;
    msg.setAddress("/viiva/sumeus");
    msg.addFloatArg(sumeus.back());
    msg.addFloatArg(sumeus.keskiarvot.back());
    msg.addFloatArg(sumeus.keskihajonnat.back());
    msg.addFloatArg(sumeus.konvergenssit.back());
    return msg;
}

void Viiva::nollaaLaskurit() {
    improvisaatioLaskin = 0;
    improvisaatioValmis = false;
    kalibraatioValmis = false;
    lahestymisLaskuri = 0;
    lahestyKohdettaValmis = false;
}

void Viiva::tyhjennaOminaisuudet() {
    paksuus = ViivanOminaisuus();
    sumeus = ViivanOminaisuus();
    paine = ViivanOminaisuus();
    pisteet.clear();
    varit.clear();
}


void Viiva::asetaKohde(shared_ptr<Viiva> kohde_) {
    kohde = kohde_;
}

OminaisuusSiivu ViivanOminaisuus::haeSiivu(int id) {
    OminaisuusSiivu siivu;
    if (id < size()) {
        siivu.arvo = arvot[id];
        siivu.keskiarvo = keskiarvot[id];
        siivu.keskihajonta = keskihajonnat[id];
        siivu.keskihajonnanKeskihajonta = keskihajonnanKeskihajonnat[id];
        siivu.konvergenssi = konvergenssit[id];
    }
    return siivu;
}

void Viiva::resize(int i) {
    pisteet.resize(i);
    varit.resize(i);
    vaiheet.resize(i);
    paine.resize(i);
    paksuus.resize(i);
    sumeus.resize(i);
}
