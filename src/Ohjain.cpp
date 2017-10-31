#include "Ohjain.h"

void Ohjain::setup() {
    Vaiheet::setup();
    Monitori::setup();

    ViivaOhjain::setup("arkisto/", "tallennetut/");

    cout << pankki.viivat.size() << " viivaa ladattu\n";

    if (OscInterface::setAddressAndPortsFromFile("./oscSettings")) {
        cout << "ladattiin oscSettings\n";
        OscInterface::connect();
    }
    //tallennetaanko kalibraatioita:
    //tallennetaan = true;

    //näkyykö viiva: (paljasta / piilota)
    Monitori::paljasta();
    //Monitori::piilota();
    Tilat::tila = Rajaa;
}

void Ohjain::updateMonitori() {

    if (Kyna::drag && Tilat::tila == Piirtaa) {
        monitoriVari = ofColor();
        //        monitoriVari = ViivaOhjain::pankki.viivaNyt.haeVari();
    } else {
        if (monitoriVari.getLightness() < 100)
            monitoriVari.setBrightness(monitoriVari.getBrightness() - 1);
        else {
            monitoriVari.setBrightness(monitoriVari.getBrightness() + 1);
            monitoriVari.setSaturation(monitoriVari.getSaturation() - 1);
        }
    }

    if (Tilat::tila == Soittaa)
        monitoriVari = pankki.viivaNyt.haeVari();
    if (Tilat::tila == Rajaa) {
        //Monitori::piirraKartta(  ) );
    }
    else {
        Monitori::piirraVari(monitoriVari);
        Monitori::piirraViiva(ViivaOhjain::pankki.viivaNyt);    
    }

}

void Ohjain::update() {
    /*jaetaan update Tilan mukaan. Tilan vaihtoehdot: rajaa, piirtaa, soittaa. update ajaa jonkun edellä mainituista funktioista */
    cout << "tila:" << Tilat::toString() << "\n";
    Tilat::update();

    // tehdään OSC paketin lähetys
    updateOSC();

}

void Ohjain::piirtaa() {

    Vaiheet::verbose();
    Vaiheet::update();

    Monitori::piirraVari(ViivaOhjain::pankki.viivaNyt.haeVari());
    Monitori::piirraViiva(ViivaOhjain::pankki.viivaNyt);
}

void Ohjain::soittaa() {


    ViivaOhjain::soita();

    cout << ofToString(soitettava.size()) << "\n";

    if (soitettava.size()) {
        Monitori::piirraVari(ViivaOhjain::soitettava.haeVari());
        Monitori::piirraViiva(ViivaOhjain::soitettava);
    }
}

void Ohjain::rajaa() {
    
    pankki.valitseViivat(savyAlue,saturaatioAlue,kirkkausAlue,paksuusAlue,sumeusAlue);
    
    //piirretään kartta
    Monitori::piirraKartta(ViivaOhjain::pankki.viivat);
}

VaiheetEnum Ohjain::kulje() {
    //jos painetaan kynällä, aloitetaan kalibrointi
    if (Kyna::click) {
        return Kalibroi;
    }

    ViivaOhjain::kulkeminen();

    //muuten jatketaan tässä vaiheessa eikä näytetä mitään
    return Kulje;
}

VaiheetEnum Ohjain::kalibroi() {
    //jos nostetaan kynä joksikin aikaa, kalibrointi keskeytetään ja palataan alkuun:
    static int irrotuslaskenta = 0;
    if (!Kyna::drag) {
        irrotuslaskenta++;
        if (irrotuslaskenta > 100) {
            irrotuslaskenta = 0;
            return Keskeyta;
        }
    } else
        irrotuslaskenta = 0;

    //onko kalibroitu onnistuneesti?
    bool kalibrointiValmis = false;

    //Kalibroidaan. Jos painekynä on käytössä, käytetään painedataa, muuten asetetaan paine=1
    if (hidpen::isOpen)
        kalibrointiValmis = ViivaOhjain::kalibrointi(Kyna::paikka, Kyna::paine);
    else
        kalibrointiValmis = ViivaOhjain::kalibrointi(Kyna::paikka, 1);

    //Jos kalibrointi päättyi onnistuneesti, edetään seuraavaan vaiheeseen
    if (kalibrointiValmis) {
        //tallenna viiva ja kuva
        if (tallennetaan) {
            ViivaOhjain::tallennaKalibrointi();
            //            Monitori::tallennaKuvana(tallennusHakemisto + "kuvat/kalibroinnit/" + tiedosto::aika() + ".png");
        }
        cout << "kalibroitu\n";
        return Improvisoi;
    }

    //Palautetaan seuraavana vaiheena VaiheetEnum Kalibroi, eli pysytään kalibrointivaiheessa
    return Kalibroi;
}

VaiheetEnum Ohjain::improvisoi() {

    //jos nostetaan kynä joksikin aikaa, palataan alkuun. TODO: toimiiko tämä ihan oikein?

    cout << "aloitetaan impro\n";

    static int irrotuslaskenta = 0;
    if (!Kyna::drag) {
        irrotuslaskenta++;
        if (irrotuslaskenta > 100) {
            irrotuslaskenta = 0;
            return Keskeyta;
        }
    } else
        irrotuslaskenta = 0;

    cout << "irrotuslaskenta tehty\n";


    bool improvisointiValmis;

    if (hidpen::isOpen)
        improvisointiValmis = ViivaOhjain::improvisointi(Kyna::paikka, Kyna::paine);
    else
        improvisointiValmis = ViivaOhjain::improvisointi(Kyna::paikka, 0.5);

    cout << "improvisointi tehty\n";

    if (improvisointiValmis) {
        return LaskeKohde;
    }

    return Improvisoi;
}

VaiheetEnum Ohjain::laskeKohde() {

    //aseta kalibrointi uusiksi, jotta ei tule värihyppäystä
    return LahestyKohdetta;
}

VaiheetEnum Ohjain::lahestyKohdetta() {
    //jos nostetaan kynä joksikin aikaa, palataan alkuun. TODO: toimiiko tämä ihan oikein?
    static int irrotuslaskenta = 0;

    if (!Kyna::drag) {
        irrotuslaskenta++;
        if (irrotuslaskenta > 100) {
            irrotuslaskenta = 0;
            return Keskeyta;
        }
    } else
        irrotuslaskenta = 0;

    bool lahestyminenValmis;

    lahestyminenValmis = ViivaOhjain::lahesty(Kyna::paikka, Kyna::paine);

    if (lahestyminenValmis) {
        return Viimeistele;
    }

    return LahestyKohdetta;
}

VaiheetEnum Ohjain::viimeistele() {

    pankki.tallennaHakemistoon("valmiitViivat/");

    //pankki.leikkaaMuokattava(pankki.muokattava.OTANNAN_KOKO);


    //Monitori::tyhjenna();
    return laskeKohde();
}

VaiheetEnum Ohjain::keskeyta() {
    //tallennetaan kuva hylättävästä viivasta
    //    Monitori::tallennaKuvana(tallennusHakemisto + "kuvat/kokonaiset/" + tiedosto::aika() + ".png");
    //    cout << tallennusHakemisto + "kuvat/kokonaiset/" + tiedosto::aika() + ".png\n";
    //    pankki.tallennaHakemistoon("keskeytetytViivat/");


    Monitori::tyhjenna();
    ViivaOhjain::pankki.aloitaUusiViivaNyt();
    cout << "uusiViiva Aloitettu\n";

    return Kulje;
}

void Ohjain::keyPressed(int key) {

    if (key == OF_KEY_TAB)
        Tilat::vaihdaTilaa();


    if (Tilat::tila == Piirtaa) {

    }
    if (Tilat::tila == Soittaa) {
        if (key == OF_KEY_LEFT)
            ViivaOhjain::edellinenViiva();
        else if (key == OF_KEY_RIGHT)
            ViivaOhjain::seuraavaViiva();
    }
    if (Tilat::tila == Rajaa) {

    }
}

void Ohjain::updateOSC() {

    // seg faulttaa tällä hetkellä
  //  cout << "lähetetään Osc paketti\n";
    OscViiva::sendViiva(pankki.viivaNyt);
  //  cout << "Osc paketti lähetetty\n";
}

