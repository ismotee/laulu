#include "ViivaOhjain.h"
#include "tilastot.h"
#include "Vaiheet.h"

void ViivaOhjain::setup(string hakemisto_, string tallennusHakemisto_) {
    pankki.lataaArkisto(hakemisto_);
    hakemisto = hakemisto_;
    tallennusHakemisto = tallennusHakemisto_;
    nykyinenPolku = 0;
    polkuLaskuri = 0;

    //luodaan hakemistot jos niitä ei ole
    ofDirectory loaddir(hakemisto_);
    if (!loaddir.exists()) {
        cout << "hakemistoa " << hakemisto_ << " ei ole! Luodaan\n";
        loaddir.create();
    }

    ofDirectory savedir(tallennusHakemisto_);
    if (!savedir.exists()) {
        cout << "hakemistoa " << hakemisto_ << " ei ole! Luodaan\n";
        savedir.create();
    }

    pankki.aloitaUusiViivaNyt();

    lukupaa = 1;
    soitettava_id = 0;
    soitettavaPlayback_id = 0;
    lukupaaPlayback = 0;

    if (!pankki.empty()) {
        soitettava = pankki[0];
    }
}

bool ViivaOhjain::kalibrointi(ofPoint paikka, float paine) {

    pankki.lisaaPiste(paikka, paine, Kalibroi);

    return pankki.viivaNyt.kalibraatioValmis;
}

bool ViivaOhjain::improvisointi(ofPoint paikka, float paine) {

    pankki.lisaaPiste(paikka, paine, Improvisoi);

    //laskee muokattavan ja kalibraation erotuksen ja lisää sen hsl:ään

    return pankki.viivaNyt.improvisaatioValmis;
}

bool ViivaOhjain::laskeKohde(ofPoint paikka, float paine) {
    pankki.lisaaPiste(paikka, paine, LaskeKohde);
    return true;
}

void ViivaOhjain::tallennaKalibrointi() {
    //jos luku- ja tallennushakemistot ovat samat, lisätään viiva myös pankkiin niin että se vaikuttaa heti ohjelman toimintaan
    //    if(hakemisto == tallennusHakemisto)
    //        pankki.lisaaMuokattavaPankkiin();

    //muuten tallennetaan se eri paikkaan
    //    pankki.tallennaHakemistoon(tallennusHakemisto);
}

bool ViivaOhjain::lahesty(ofPoint paikka, float paine) {
    pankki.lisaaPiste(paikka, paine, LahestyKohdetta);
    return pankki.viivaNyt.lahestyKohdettaValmis;
}

bool ViivaOhjain::kulkeminen() {
    //ofRandom(0,pankki.size());
}

void ViivaOhjain::soita() {
    if (pankki.valitutViivat.empty())
        return;
    lukupaa++;
    //    if(lukupaa >= pankki.valitutViivat[soitettava_id].size())
    if (lukupaa >= 10000)
        lukupaa = 1;

    //cout << "koko: " << lukupaa << "/" << pankki.valitutViivat[soitettava_id].size() << "\n";


    soitettava = pankki.valitutViivat[soitettava_id];

    soitettava.resize(lukupaa);
}

void ViivaOhjain::meneAlkuun() {
    lukupaa = 0;
}

void ViivaOhjain::edellinenViiva() {
    soitettava_id--;
    if (soitettava_id < 0)
        soitettava_id = pankki.valitutViivat.size() - 1;
}

void ViivaOhjain::seuraavaViiva() {
    soitettava_id++;
    if (soitettava_id >= pankki.valitutViivat.size())
        soitettava_id = 0;
}

void ViivaOhjain::lataaAani() {
    sPlayer.load("tallennetut/aanet/" + soitettava.nimi + ".wav");
    
}

void ViivaOhjain::aloitaAani(int kohta) {
    sPlayer.setPositionMS(kohta / 60 * 1000);
    sPlayer.play();
}

void ViivaOhjain::lopetaAani() {
    sPlayer.stop();
}

const Viiva& ViivaOhjain::haeKalibrointi() const {

}

const Viiva& ViivaOhjain::haeMuokattava() const {

}

void ViivaOhjain::playbackAlkuun() {
    lukupaaPlayback = 0;
}

int ViivaOhjain::soitaPlayback() {
    lukupaaPlayback++;


    if (!pankki.piirretytViivat.empty() && lukupaaPlayback >= pankki.piirretytViivat[soitettavaPlayback_id].size()) {
        lukupaaPlayback = 0;
    }
    return lukupaaPlayback;

}

void ViivaOhjain::edellinenViivaPlayback() {
    soitettavaPlayback_id--;

    soitettavaPlayback_id = ofWrap(soitettavaPlayback_id, 0, pankki.piirretytViivat.size());

    if (!pankki.piirretytViivat.empty())
        soitettava = pankki.piirretytViivat[soitettavaPlayback_id];

    sPlayer.load("tallennetut/aanet/" + soitettava.nimi + ".wav");

    playbackAlkuun();
}

void ViivaOhjain::seuraavaViivaPlayback() {
    soitettavaPlayback_id++;
    soitettavaPlayback_id = ofWrap(soitettavaPlayback_id, 0, pankki.piirretytViivat.size());

    if (!pankki.piirretytViivat.empty())
        soitettava = pankki.piirretytViivat[soitettavaPlayback_id];

    sPlayer.load("tallennetut/aanet/" + soitettava.nimi + ".wav");

    playbackAlkuun();
}

bool ViivaOhjain::viimeistele() {
    pankki.viimeistelePiirto();
    pankki.tallennaHakemistoon(tallennusHakemisto);
    return true;
}
