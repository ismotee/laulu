#include "ViivaOhjain.h"
#include "tilastot.h"
#include "Vaiheet.h"

void ViivaOhjain::setup(string hakemisto_, string tallennusHakemisto_) {
    pankki.lataaHakemistosta(hakemisto_);
    hakemisto = hakemisto_;
    tallennusHakemisto = tallennusHakemisto_;
    nykyinenPolku = 0;
    polkuLaskuri = 0;
    
    //luodaan hakemistot jos niitä ei ole
    ofDirectory loaddir(hakemisto_);
    if (!loaddir.exists() ) {
        cout << "hakemistoa " << hakemisto_ << " ei ole! Luodaan\n";
        loaddir.create();
    }

    ofDirectory savedir(tallennusHakemisto_);
    if (!savedir.exists() ) {
        cout << "hakemistoa " << hakemisto_ << " ei ole! Luodaan\n";
        savedir.create();
    }
    
    pankki.aloitaUusiViivaNyt();
    
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

bool ViivaOhjain::laskeKohdeVari() {



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

}
