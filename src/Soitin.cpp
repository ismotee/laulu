#include "Soitin.h"

Soitin::Soitin() : lukupaa(0), nykyinen_id(0) {
}

void Soitin::setup(std::vector<Viiva>* viivat_ptr) {
    viivat = viivat_ptr;
}

void Soitin::lataa(std::string polku) {

    ofDirectory dir(polku);

    if (!dir.exists()) {
        cout << "ei polkua: " << dir.getAbsolutePath();
        return;
    }

    dir.listDir();

    for (int i = 0; i << dir.size(); i++)
        viivat->push_back(tiedosto::lataaViiva(dir[i].getAbsolutePath()));
}

void Soitin::tallenna(std::string polku) {

    ofDirectory td(polku);
    if (!td.exists())
        td.createDirectory(polku);

    tiedosto::tallennaViiva(nykyinen, polku);
}

void Soitin::edellinenViiva() {
    uusiViiva = true;
    if (viivat->empty()) {
        nykyinen_id = 0;
        return;
    }

    nykyinen_id--;
    if (nykyinen_id < 0)
        nykyinen_id = viivat->size() - 1;

    nykyinen = viivat->at(nykyinen_id);
}

void Soitin::seuraavaViiva() {
    uusiViiva = true;
    if (viivat->empty()) {
        nykyinen_id = 0;
        return;
    }

    nykyinen_id++;
    if (nykyinen_id == viivat->size())
        nykyinen_id = 0;

    nykyinen = viivat->at(nykyinen_id);
}

Viiva Soitin::nykyinenViiva() {
    return nykyinen;
}

void Soitin::katkaiseViivaKohdasta(int kohta) {

    nykyinen = viivat->at(nykyinen_id);
    
    if (kohta > 1) {
        nykyinen.pisteet.resize(kohta - 1);
        nykyinen.sumeus.resize(kohta - 1);
        nykyinen.paksuus.resize(kohta - 1);
    }
    nykyinen.lisaaPiste(viivat->at(nykyinen_id).pisteet[kohta].sijainti,viivat->at(nykyinen_id).pisteet[kohta].paine);
}

Viiva Soitin::soita() {
    uusiViiva = false;
    if (nykyinen_id >= viivat->size()) {
        cout << "Soitin: nykyinen_id osoittaa tyhjään kohtaan.\n";
        return Viiva();
    }

    lukupaa++;

    if (viivat->at(nykyinen_id).pisteet.size() <= lukupaa) {
        lukupaa = 0;
        return Viiva();
    }

    katkaiseViivaKohdasta(lukupaa);
    return nykyinen;
}
