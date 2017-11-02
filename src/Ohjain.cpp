#include "Ohjain.h"

void Ohjain::setup() {
    Vaiheet::setup();
    monitori1.setup();

    ViivaOhjain::setup("arkisto/", "tallennetut/");

    cout << pankki.viivat.size() << " viivaa ladattu\n";

    if (OscInterface::setAddressAndPortsFromFile("./oscSettings")) {
        cout << "ladattiin oscSettings\n";
        OscInterface::connect();
    }
    //tallennetaanko kalibraatioita:
    //tallennetaan = true;

    //näkyykö viiva: (paljasta / piilota)
    //monitori1.paljasta();
    //monitori1.piilota();
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

    if (Tilat::tila == Soittaa && ViivaOhjain::soitettava.size()) {
        monitoriVari = ViivaOhjain::soitettava.varit[lukupaaPlayback];
        monitori2.piirraVari(monitoriVari);
    }
    if (Tilat::tila == Rajaa) {
        ViivaOhjain::soita();
        if (valintaMuuttui)
            monitori2.tyhjenna();
        monitori2.piirraViivatKohdasta(pankki.valitutViivat, ViivaOhjain::lukupaa);
    } else if (Tilat::tila == Piirtaa) {

        vector<Viiva> v(1, ViivaOhjain::pankki.viivaNyt);
        monitori2.piirraViivatLopusta(v);
    }


}

void Ohjain::update() {
    /*jaetaan update Tilan mukaan. Tilan vaihtoehdot: rajaa, piirtaa, soittaa. update ajaa jonkun edellä mainituista funktioista */
    Tilat::update();

    // tehdään OSC paketin lähetys
    updateOSC();

    //kokeillaan valintakriteerejä
    //savyAlue.min += 0.1;
    //savyAlue.max += 0.1;

}

void Ohjain::piirtaa() {

    Vaiheet::verbose();
    Vaiheet::update();

    if (Vaiheet::vaiheetEnum == Kulje) {
        monitori1.tyhjenna();
        monitori1.piirraKartta(pankki.valitutViivat, 10);

    } else if (!pankki.viivaNyt.empty()) {
        //Monitori::piirraVari(ViivaOhjain::pankki.viivaNyt.haeVari());
        vector<Viiva> v(1, ViivaOhjain::pankki.viivaNyt);
        monitori1.piirraViivatKohdasta(v, pankki.viivaNyt.size() - 1);
    }
}

void Ohjain::soittaa() {

    


    if (ViivaOhjain::soitettava.size()) {
        //monitori1.piirraVari(ViivaOhjain::soitettava.haeVari());
        vector<Viiva> v(1,soitettava);
        int l= ViivaOhjain::soitaPlayback();
        if(l == 0)
            monitori1.tyhjenna();
        monitori1.piirraVari(soitettava.varit[l]);
        monitori1.piirraViivatAlusta(v, l);
    }
}

void Ohjain::rajaa() {
    //Kaikki viivat pienellä ja valitut viivat isolla
    monitori1.tyhjenna();
    monitori1.piirraKartta(ViivaOhjain::pankki.viivat, 3);
    monitori1.piirraKartta(ViivaOhjain::pankki.valitutViivat, 10);

    if (Kyna::click) {
        //Kynasta saa suoraan paksuus-sumeus-vektorin:
        valintaMuuttui = true;
        ViivaOhjain::meneAlkuun();
        ViivaOhjain::pankki.toglaaValinta(Kyna::paikka_scaled);
    } else
        valintaMuuttui = false;
}

VaiheetEnum Ohjain::kulje() {
    //jos painetaan kynällä, aloitetaan kalibrointi
    if (Kyna::click) {
        pankki.valitseVari(Kyna::paikka_scaled);
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
            //            monitori1.tallennaKuvana(tallennusHakemisto + "kuvat/kalibroinnit/" + tiedosto::aika() + ".png");
        }
        cout << "kalibroitu\n";
        return Improvisoi;
    }

    //Palautetaan seuraavana vaiheena VaiheetEnum Kalibroi, eli pysytään kalibrointivaiheessa
    return Kalibroi;
}

VaiheetEnum Ohjain::improvisoi() {

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



    bool improvisointiValmis;

    if (hidpen::isOpen)
        improvisointiValmis = ViivaOhjain::improvisointi(Kyna::paikka, Kyna::paine);
    else
        improvisointiValmis = ViivaOhjain::improvisointi(Kyna::paikka, 0.5);


    if (improvisointiValmis) {
        return LaskeKohde;
    }

    return Improvisoi;
}

VaiheetEnum Ohjain::laskeKohde() {
    ViivaOhjain::laskeKohde(Kyna::paikka, Kyna::paine);
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

    if (hidpen::isOpen)
        lahestyminenValmis = ViivaOhjain::lahesty(Kyna::paikka, Kyna::paine);
    else
        lahestyminenValmis = ViivaOhjain::lahesty(Kyna::paikka, 0.5);

    if (lahestyminenValmis) {
        return Viimeistele;
    }

    return LahestyKohdetta;
}

VaiheetEnum Ohjain::viimeistele() {

    //pankki.tallennaHakemistoon("valmiitViivat/");
    pankki.viivaNyt.nollaaLaskurit();
    //pankki.leikkaaMuokattava(pankki.muokattava.OTANNAN_KOKO);


    //monitori1.tyhjenna();
    return laskeKohde();
}

VaiheetEnum Ohjain::keskeyta() {
    //tallennetaan kuva hylättävästä viivasta
    //    monitori1.tallennaKuvana(tallennusHakemisto + "kuvat/kokonaiset/" + tiedosto::aika() + ".png");
    //    cout << tallennusHakemisto + "kuvat/kokonaiset/" + tiedosto::aika() + ".png\n";
    //    pankki.tallennaHakemistoon("keskeytetytViivat/");

    

    monitori1.tyhjenna();
    ViivaOhjain::viimeistele();
    ViivaOhjain::pankki.aloitaUusiViivaNyt();
    cout << "uusiViiva Aloitettu\n";

    return Kulje;
}

void Ohjain::keyPressed(int key) {

    if (key == OF_KEY_TAB) {
        Tilat::vaihdaTilaa();
        cout << "tila:" << Tilat::toString() << "\n";
    }

    if (Tilat::tila == Piirtaa) {

    }
    if (Tilat::tila == Soittaa) {
        if (key == OF_KEY_LEFT)
            ViivaOhjain::edellinenViivaPlayback();
        else if (key == OF_KEY_RIGHT)
            ViivaOhjain::seuraavaViivaPlayback();
        cout << ViivaOhjain::soitettavaPlayback_id << "\n";
    }
    if (Tilat::tila == Rajaa) {

    }
}

void Ohjain::updateOSC() {

    // seg faulttaa tällä hetkellä
    //  cout << "lähetetään Osc paketti\n";
    OscViiva::sendViiva(pankki.viivaNyt,Vaiheet::vaiheetEnum);
    //  cout << "Osc paketti lähetetty\n";
}

