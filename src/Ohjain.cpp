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

    ViivaOhjain::pankki.aloitaUusiMuokattava();
    ViivaOhjain::pankki.aloitaUusiKalibrointi();
    ViivaOhjain::arvoMuokattavanVari();
}

void Ohjain::updateMonitori() {

    if (Kyna::drag && Tilat::tila == Piirtaa) {
        monitoriVari = ofColor();
        monitoriVari = ViivaOhjain::haeMuokattava().vari;
    } else {
        if (monitoriVari.getLightness() < 100)
            monitoriVari.setBrightness(monitoriVari.getBrightness() - 1);
        else {
            monitoriVari.setBrightness(monitoriVari.getBrightness() + 1);
            monitoriVari.setSaturation(monitoriVari.getSaturation() - 1);
        }

    }

    if(Tilat::tila == Soittaa)
        monitoriVari = pankki.nykyinen.vari;
    
    Monitori::piirraVari(monitoriVari);
    Monitori::piirraViiva(ViivaOhjain::haeMuokattava());


}

void Ohjain::update() {
    /*jaetaan update Tilan mukaan. Tilan vaihtoehdot: rajaa, piirtaa, soittaa. update ajaa jonkun edellä mainituista funktioista */
    Tilat::update();
    
    // tehdään OSC paketin lähetys
    updateOSC();
    
}

void Ohjain::piirtaa() {
    Vaiheet::update();

    Monitori::piirraVari(ViivaOhjain::haeMuokattava().vari);
    Monitori::piirraViiva(ViivaOhjain::haeMuokattava());
    Vaiheet::verbose();
}

void Ohjain::soittaa() {
    
    if(pankki.uusiViiva)
        Monitori::tyhjenna();
    
    Viiva v = pankki.soita();
    
    Monitori::piirraVari(v.vari);
    Monitori::piirraViiva(v);
}

void Ohjain::rajaa() {

}


VaiheetEnum Ohjain::kulje() {
    //jos painetaan kynällä, aloitetaan kalibrointi
    if (Kyna::click) {
        return Improvisoi;
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
    bool kalibrointiValmis;

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
            Monitori::tallennaKuvana(tallennusHakemisto + "kuvat/kalibroinnit/" + tiedosto::aika() + ".png");
        }
        aloitaImprovisointi();
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
        improvisointiValmis = ViivaOhjain::improvisointi(Kyna::paikka, 1);

    if (improvisointiValmis) {
        return LaskeKohde;
    }

    return Improvisoi;
}

VaiheetEnum Ohjain::laskeKohde() {

    //aseta kalibrointi uusiksi, jotta ei tule värihyppäystä
    ViivaOhjain::pankki.muokattava.asetaAlkuperainenVari();
    pankki.kalibrointi = pankki.muokattava;
    ViivaOhjain::lahestymisLaskuri = 0;
    ViivaOhjain::muutos.clear();


    if (pankki.viivat.size() < 10) {
        return Kulje;
    }


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

    ViivaOhjain::pankki.muokattava.asetaAlkuperainenVari();
    //pankki.leikkaaMuokattava(pankki.muokattava.OTANNAN_KOKO);
    ViivaOhjain::pankki.kalibrointi = ViivaOhjain::pankki.muokattava;


    //Monitori::tyhjenna();
    return laskeKohde();
}

VaiheetEnum Ohjain::keskeyta() {
    //tallennetaan kuva hylättävästä viivasta
    Monitori::tallennaKuvana(tallennusHakemisto + "kuvat/kokonaiset/" + tiedosto::aika() + ".png");
    cout << tallennusHakemisto + "kuvat/kokonaiset/" + tiedosto::aika() + ".png\n";
    pankki.tallennaHakemistoon("keskeytetytViivat/");


    Monitori::tyhjenna();
    ofColor vari = pankki.muokattava.vari;
    ViivaOhjain::pankki.aloitaUusiMuokattava();
    ViivaOhjain::pankki.aloitaUusiKalibrointi();
    //ViivaOhjain::arvoMuokattavanVari();
    pankki.muokattava.vari = vari;
    pankki.muokattava.alkuperainenVari = vari;
    return Kulje;
}

void Ohjain::keyPressed(int key) {
    
    if(key == OF_KEY_TAB)
        Tilat::vaihdaTilaa();
    
    
    if(Tilat::tila == Piirtaa) {
        
    }
    if(Tilat::tila == Soittaa) {
        if(key == OF_KEY_LEFT)
            pankki.edellinenViiva();
        else if(key == OF_KEY_RIGHT)
            pankki.seuraavaViiva();
    }
    if(Tilat::tila == Rajaa) {
        
    }
}




void Ohjain::updateOSC() {

    // seg faulttaa tällä hetkellä
    
    
        ofxOscMessage msg;
        msg.setAddress("/viiva/vaihe");
        msg.addStringArg(Vaiheet::toString());

        OscInterface::sendMessage(msg);

        if (Vaiheet::vaiheetEnum != Kulje) {
            OscInterface::sendMessage(pankki.muokattava.makePisteAsOscMessage());
            OscInterface::sendMessage(pankki.muokattava.makePaksuusAsOscMessage());
            OscInterface::sendMessage(pankki.muokattava.makeSumeusAsOscMessage());
        }
    

}

