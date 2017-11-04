#include "ViivaPankki.h"

float wrap(float f, float max) {
    if (f > max) {
        int n = (int) (f / max);
        f -= n * max;
    }
    if (f < 0) {
        f *= -1;
        //poistetaan max:n monikerrat
        int n = (int) (f / max);
        f -= n*max;
        f *= -1; //käännetään takaisin. Tämän jälkeen f on [-max ... 0]
        f += max; //nyt pitäisi olla [0...max]
    }
    return f;
}

bool Alue::onAlueella(float f) const {
    if (f < min) return false;
    else if (f > max) return false;
    else return true;
}

bool Alue::onAlueella_wrapped(float f, float wrapLimit) const {
    float r = this->range();
    float P = this->keskipiste();

    P = wrap(P, wrapLimit);
    f = wrap(f, wrapLimit);

    if (P >= r && P <= wrapLimit - r) {
        if (f > P - r && f < P + r) return true;
        else return false;
    } else if (P < r) {
        if (f > P - r + wrapLimit || f < P + r) return true;
        else return false;
    } else if (P > wrapLimit - r) {
        if (f > P - r || f < P + r - wrapLimit) return true;
        else return false;
    }
}

float Alue::keskipiste() const {
    return range() + min;
}

float Alue::range() const { //maksimietäisyys keskipisteestä
    return (max - min) / 2;
}

void Alue::asetaKeskipisteenMukaan(float p, float r) {
    min = p - r;
    max = p + r;
}

ofVec2f viivaSortti::keskipiste(0, 0);

bool viivaSortti::onLahempana(const Viiva& lhs, const Viiva& rhs) {
    //return true if lhs < rhs
    ofVec2f d_l = keskipiste - lhs.kalibraatio.paksuusSumeusKeskiarvoVektori();
    ofVec2f d_r = keskipiste - rhs.kalibraatio.paksuusSumeusKeskiarvoVektori();
    return (d_l.length() < d_r.length());
}

void viivaSortti::jarjesta(std::vector<Viiva>& viivat, ofVec2f P) {
    keskipiste = P;
    std::sort(viivat.begin(), viivat.end(), onLahempana);
}

bool viivojenInterpoloija::onkoPisteKolmionSisalla(ofVec2f P) {
    ofVec3f AP = P - kolmio[0].kalibraatio.paksuusSumeusKeskiarvoVektori();
    ofVec3f AB = kolmio[1].kalibraatio.paksuusSumeusKeskiarvoVektori() - kolmio[0].kalibraatio.paksuusSumeusKeskiarvoVektori();

    ofVec3f BP = P - kolmio[1].kalibraatio.paksuusSumeusKeskiarvoVektori();
    ofVec3f BC = kolmio[2].kalibraatio.paksuusSumeusKeskiarvoVektori() - kolmio[1].kalibraatio.paksuusSumeusKeskiarvoVektori();

    ofVec3f CP = P - kolmio[2].kalibraatio.paksuusSumeusKeskiarvoVektori();
    ofVec3f CA = kolmio[0].kalibraatio.paksuusSumeusKeskiarvoVektori() - kolmio[2].kalibraatio.paksuusSumeusKeskiarvoVektori();

    ofVec3f v1 = AP.crossed(AB);
    ofVec3f v2 = BP.crossed(BC);
    ofVec3f v3 = CP.crossed(CA);

    if (v1.dot(v2) > 0) {
        if (v1.dot(v3) > 0)
            return true;
    }
    return false;

}

bool viivojenInterpoloija::etsiKolmio(std::vector<Viiva> viivat, ofVec2f P) {
    if (viivat.size() < 3) return false;

    if (onkoPisteKolmionSisalla(P))
        return true;

    //järjestetään ja laitetaan lähimmät kolmioon
    viivaSortti::jarjesta(viivat, P);
    std::copy(viivat.begin(), viivat.begin() + 3, kolmio);

    bool found = false;

    //ruvetaan katsomaan onko piste kolmion sisällä

    kolmio[0] = viivat[0];
    for (int i = 1; i < viivat.size() && !found; i++) {
        kolmio[1] = viivat[i];
        for (int j = i + 1; j < viivat.size() && !found; j++) {
            kolmio[2] = viivat[j];
            if (onkoPisteKolmionSisalla(P)) {
                found = true;
            }
        }
    }

    //katsotaan löytyikö jotain
    return found;
}

ofColor viivojenInterpoloija::interpoloiVari(ofVec2f P) {

    if (!onkoPisteKolmionSisalla(P))
        return ofColor();

    ofVec2f AB = kolmio[1].kalibraatio.paksuusSumeusKeskiarvoVektori() - kolmio[0].kalibraatio.paksuusSumeusKeskiarvoVektori();
    ofVec2f AC = kolmio[2].kalibraatio.paksuusSumeusKeskiarvoVektori() - kolmio[0].kalibraatio.paksuusSumeusKeskiarvoVektori();
    ofVec2f BC = kolmio[2].kalibraatio.paksuusSumeusKeskiarvoVektori() - kolmio[1].kalibraatio.paksuusSumeusKeskiarvoVektori();
    ofVec2f BP = P - kolmio[1].kalibraatio.paksuusSumeusKeskiarvoVektori();
    ofVec2f AP = P - kolmio[0].kalibraatio.paksuusSumeusKeskiarvoVektori();

    float l_B = BP.dot(BC);
    ofVec2f BPdot = (l_B / BC.length()) * BC;
    ofVec2f APdot = AB + BPdot;

    float l_A = AP.length() / APdot.length();

    ofColor V_p_dot = kolmio[1].kalibraatio.vari * l_B + kolmio[2].kalibraatio.vari * (1 - l_B);

    return kolmio[0].kalibraatio.vari * l_A + V_p_dot * (1 - l_A);
    // that's  it. we're done!

}

bool ViivaPankki::lataaArkisto(const std::string& polku) {

    arkisto = ofDirectory(polku);
    if (!arkisto.exists()) {
        std::cout << "ViivaPankki:lataaArkisto(): ei polkua " << polku << "\n";
        return false;
    }

    arkisto.listDir();
    sarja_i = 0;
    valitseSarja(sarja_i);
    return true;
}

bool ViivaPankki::valitseSarja(int i) {
    if (!arkisto.exists() || arkisto.size() == 0) {
        return false;
    }

    //jos annettiin huono indeksi:
    if (i >= arkisto.size() || i < 0)
        return false;

    tyhjennaValinta();
    std::string polku = arkisto.getPath(sarja_i);
    cout << "valitse Sarja: polku: " << polku << "\n";
    lataaHakemistosta(polku);

}

void ViivaPankki::seuraavaSarja() {
    sarja_i++;
    sarja_i = ofWrap(sarja_i, 0, arkisto.size());
    valitseSarja(sarja_i);
}

void ViivaPankki::edellinenSarja() {
    sarja_i--;
    sarja_i = ofWrap(sarja_i, 0, arkisto.size());
    valitseSarja(sarja_i);
}

void ViivaPankki::aloitaUusiViivaNyt() {
    viivaNyt = Viiva();
}

void ViivaPankki::lisaaPiste(ofPoint paikka, float paine, VaiheetEnum vaihe) {

    if (vaihe == LaskeKohde) {

        shared_ptr<Viiva> ptr = make_shared<Viiva>(*etsiViiva());

        viivaNyt.asetaKohde(ptr);
    } else if (vaihe == LahestyKohdetta) {
        //viivaNyt.asetaVari(interpoloiVari(viivaNyt.paksuusSumeusKeskiarvoVektori()));
        laskeVari();
        viivaNyt.lisaaPiste(paikka, paine, vaihe);

    } else
        viivaNyt.lisaaPiste(paikka, paine, vaihe);
}

Viiva* ViivaPankki::etsiViiva() {
    //etsitään viiva, joka on lähimmäksi sitä suuntaa, johon on liikuttu kalibraation jälkeen.
    /*
    ofVec2f kalibrointiVec = viivaNyt.kalibraatio.paksuusSumeusVektori();
    ofVec2f muokattavaVec = viivaNyt.paksuusSumeusVektori();
    ofVec2f vertailuVec = (muokattavaVec - kalibrointiVec).getNormalized();
    float nearestValue = -100;
    int nearestId = -1;

    vector<float> samankaltaisuus(valitutViivat.size(), 0);

    // lasketaan samankaltaisuus
    for (int i = 0; i < valitutViivat.size(); i++) {
        ofVec2f vec = valitutViivat[i].kalibraatio.paksuusSumeusVektori();
        ofVec2f suunta = (vec - kalibrointiVec).getNormalized();

        samankaltaisuus[i] += suunta.dot(vertailuVec);
    }


    for (int i = 0; i < samankaltaisuus.size(); i++) {
        if (samankaltaisuus[i] > nearestValue) {
            nearestValue = samankaltaisuus[i];
            nearestId = i;
        }
    }
     */

    float lyhin = (valitutViivat[0].paksuusSumeusKeskiarvoVektori() - viivaNyt.paksuusSumeusKeskiarvoVektori()).length();
    int lyhin_i = 0;
    for (int i = 1; i < valitutViivat.size(); i++) {
        float l = (valitutViivat[i].paksuusSumeusKeskiarvoVektori() - viivaNyt.paksuusSumeusKeskiarvoVektori()).length();
        if (l < lyhin) {
            lyhin = l;
            lyhin_i = i;
        }
    }

#ifdef VIIVA_DEBUG
    cout << "etsiViiva, nearestValue: " << nearestValue << "\n";
    cout << "etsiViiva, nearestId: " << nearestId << "\n";
#endif

    return &valitutViivat[lyhin_i];
}

void ViivaPankki::tallennaHakemistoon(string polku) {
    ofDirectory td(polku);
    if (!td.exists())
        td.createDirectory(polku);
    ofDirectory d("./");

    if (viivat.empty())return;
    tiedosto::tallennaViiva(viivaNyt, d.getAbsolutePath() + "/" + polku);
}

bool ViivaPankki::lataaHakemistosta(string polku) {
    ofDirectory dir(polku + "/viivat/ov1");
    if (!dir.exists()) {
        cout << "yritettiin ladata " << dir.getAbsolutePath() << "\n";
        return false;
    }
    cout << "ladataan " << dir.getAbsolutePath() << "\n";

    viivat.clear();
    dir.listDir();

    for (int i = 0; i < dir.size(); i++) {
        viivat.push_back(tiedosto::lataaViiva(dir[i].getAbsolutePath()));
        viivat.back().sarja = ofSplitString(polku, "/", true)[1];
    }
}

void ViivaPankki::asetaKohteeksi(int id) {
    if (id < size())
        viivaNyt.asetaKohde(make_shared<Viiva>(viivat[id]));
}

void ViivaPankki::valitseViivat(Alue savyAlue, Alue saturaatioAlue, Alue kirkkausAlue, Alue paksuusAlue, Alue sumeusAlue) {

    valitutViivat.clear();

    for (int i = 0; i < viivat.size(); i++) {
        Kalibraatio k = viivat[i].kalibraatio;

        if (!savyAlue.onAlueella_wrapped(k.vari.getHue(), 255) || !savyAlue.aktiivinen)
            continue;
        if (!saturaatioAlue.onAlueella(k.vari.getSaturation()) || !saturaatioAlue.aktiivinen)
            continue;
        if (!kirkkausAlue.onAlueella(k.vari.getBrightness()) || !kirkkausAlue.aktiivinen)
            continue;
        if (!paksuusAlue.onAlueella(k.paksuus) || !paksuusAlue.aktiivinen)
            continue;
        if (!sumeusAlue.onAlueella(k.sumeus) || !sumeusAlue.aktiivinen)
            continue;

        //jos tähän päästiin, ollaan alueilla!
        valitutViivat.push_back(viivat[i]);
    }
}

// etsitään viivaa, joka on lähimpänä annettua Paksuus-Sumeus vektoria valituista Viivaoista. Jos löytyy, poistetaan. Jos ei, lisätään.

void ViivaPankki::toglaaValinta(ofVec2f paksuusSumeusKa) {

    if (viivat.empty())
        return;

    float lyhinEtaisyys = (viivat[0].kalibraatio.paksuusSumeusKeskiarvoVektori() - paksuusSumeusKa).length();
    int lyhinId = 0;

    for (int i = 0; i < viivat.size(); i++) {

        float etaisyys = (viivat[i].kalibraatio.paksuusSumeusKeskiarvoVektori() - paksuusSumeusKa).length();

        if (etaisyys < lyhinEtaisyys) {
            lyhinEtaisyys = etaisyys;
            lyhinId = i;
        }
    }


    for (int i = 0; i < valitutViivat.size(); i++) {

        // verrataan nimeen, koska nimi on aina uniikki. 
        if (viivat[lyhinId].nimi.compare(valitutViivat[i].nimi) == 0) {
            // poistetaan viiva ja lopetetaan metodi välittömästi
            valitutViivat.erase(valitutViivat.begin() + i);
            return;
        }
    }
    // jos ei löytynyt niin lisätään viiva
    valitutViivat.push_back(viivat[lyhinId]);

}

void ViivaPankki::valitseVari(ofVec2f paksuusSumeusKa) {
    if (valitutViivat.empty())
        return;

    float lyhinEtaisyys = (valitutViivat[0].kalibraatio.paksuusSumeusKeskiarvoVektori() - paksuusSumeusKa).length();
    int lyhinId = 0;

    for (int i = 0; i < valitutViivat.size(); i++) {

        float etaisyys = (valitutViivat[i].kalibraatio.paksuusSumeusKeskiarvoVektori() - paksuusSumeusKa).length();

        if (etaisyys < lyhinEtaisyys) {
            lyhinEtaisyys = etaisyys;
            lyhinId = i;
        }
    }
    viivaNyt.varit.push_back(valitutViivat[lyhinId].kalibraatio.vari);
    cout << "valittu vari: " << ofToString(viivaNyt.varit.back()) << "\n";
}

void ViivaPankki::viimeistelePiirto() {
    piirretytViivat.push_back(viivaNyt);
}

int ViivaPankki::lataaPiirretytViivat(std::string polku) {
    ofDirectory dir = ofDirectory(polku);
    if (!dir.exists()) {
        cout << "PiirretytViivat: ei polkua. Luodaan.\n";
        dir.create(true);
        return 0;
    }
    dir.listDir();

    piirretytViivat.clear();

    for (int i = 0; i < dir.size(); i++) {
        if (!dir[i].isDirectory()) {
            piirretytViivat.push_back(tiedosto::lataaViiva(dir[i].getAbsolutePath()));
        }
    }

    return piirretytViivat.size(); 
}

void ViivaPankki::tyhjennaValinta() {
    valitutViivat.clear();
}

void ViivaPankki::laskeVari() {

    float pituus_1 = (kolmio[0].kalibraatio.paksuusSumeusKeskiarvoVektori() - viivaNyt.paksuusSumeusKeskiarvoVektori()).length();
    float pituus_2 = (kolmio[1].kalibraatio.paksuusSumeusKeskiarvoVektori() - viivaNyt.paksuusSumeusKeskiarvoVektori()).length();
    float pituus_3 = (kolmio[2].kalibraatio.paksuusSumeusKeskiarvoVektori() - viivaNyt.paksuusSumeusKeskiarvoVektori()).length();

    float sum = pituus_1 + pituus_2 + pituus_3;

    float suhdeluku_1 = (1 - pituus_1 / sum) / 2;
    float suhdeluku_2 = (1 - pituus_2 / sum) / 2;
    float suhdeluku_3 = (1 - pituus_3 / sum) / 2;



    viivaNyt.asetaVari(kolmio[0].kalibraatio.vari * suhdeluku_1 + kolmio[1].kalibraatio.vari * suhdeluku_2 + kolmio[2].kalibraatio.vari * suhdeluku_3);
}
