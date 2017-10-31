#include "tiedosto.h"
#include <iterator>
#include <iostream>
#include <time.h>
#include <fstream>
#include <vector>

Viiva tiedosto::lataaViiva(std::string tiedostonNimi) {
    Viiva result;
    
    std::ifstream file(tiedostonNimi, std::ios::binary | std::ios::in);
    if(!file.is_open() ) {
        cout << "ei tiedostoa: " << tiedostonNimi << "\n";
        return result; //ei tiedostoa!
    }
    
    //vektorit joihin tiedot tallennetaan
    std::vector<ofPoint> pisteet;
    std::vector<float> paineet;
    std::vector<float> paksuudet;
    std::vector<float> sumeudet;
    std::vector<VaiheetEnum> vaiheet;
    std::vector<ofColor> varit;
    
    ofColor kalibraationVari;

    //tiedoston alussa olevat otsikkotiedot
    int versio;
    std::string nimi;
    std::string sarja;
    
    //luetaan otsikko:
    file >> nimi >> versio >> sarja;

    //aloitetaan tiedoston lukeminen:
    std::string luettu; //tähän tulee luettu rivi
    VaiheetEnum vaihe = EiOle; //käynnissä oleva vaihe
    
    //jos ei olla missään vaiheessa, odotetaan etta joku vaihe alkaa. Muuten luetaan asioita vaiheesta riippuen
    for(;file >> luettu; file.good() ) {
        
        if(luettu.compare("vaihe")==0 ) {
            //luetaan seuraavalta riviltä vaiheen nimi ja vaihdetaan siihen
            file >> luettu;
            vaihe = tulkkaaVaihe(luettu);
            if(vaihe != EiOle) {
                std::string klo;
                //luetaan kellonaika ja väri, jos ollaan kalibroinnissa
                file >> klo;
                if(vaihe == Kalibroi) {
                    int r, g, b;
                    file >> luettu; 
                    file >> r;
                    file >> g;
                    file >> b;
                    kalibraationVari = ofColor(r, g, b);
                }
            }            
        }
        
        else if(luettu.compare("pisteet")==0) {
            int koko; //montako pistettä            
            file >> koko;
            
            //tehdään tilaa vektoriin
            int alkuKoko = pisteet.size();
            pisteet.resize(pisteet.size()+koko);
            
            //aletaan lukea pisteitä. Piste on 2 floattia (x, y)
            file.seekg(1, file.cur); //eliminoi rivinvaihto
            int bytes = koko * 2 * sizeof(float);
            float* buffer = new float[koko * 2];
            file.read((char*)buffer, bytes);
            
            //tallenna pisteet vektoriin
            for(int piste_i = alkuKoko; piste_i < alkuKoko+koko; piste_i++) {
                float x = buffer[piste_i*2];
                float y = buffer[piste_i*2 + 1];
                pisteet[piste_i] = ofPoint(x,y);
            }
            
            delete[] buffer;
            
        }

        else if(luettu.compare("paineet")==0) {
            int koko;
            file >> koko;
            
            //aletaan lukea. Paine on float
            file.seekg(1, file.cur); //eliminoi rivinvaihto
            int bytes = koko * sizeof(float);
            float* buffer = new float[koko];
            file.read((char*)buffer, bytes);

            //laitetaan luvut vektoriin
            paineet.insert(paineet.end(),buffer,buffer+koko);
            
            delete[] buffer;            
        }

        else if(luettu.compare("paksuudet")==0) {
            int koko;
            file >> koko;
            
            file.seekg(1, file.cur); //eliminoi rivinvaihto
            int bytes = koko * sizeof(float);
            float* buffer = new float[koko];
            file.read((char*)buffer, bytes);
            
            paksuudet.insert(paksuudet.end(),buffer,buffer+koko);
            delete[] buffer;
        }

        else if(luettu.compare("sumeudet")==0) {
            int koko;
            file >> koko;

            file.seekg(1, file.cur); //eliminoi rivinvaihto
            int bytes = koko * sizeof(float);
            float* buffer = new float[koko];
            file.read((char*)buffer, bytes);
            
            sumeudet.insert(sumeudet.end(),buffer,buffer+koko);
            
            delete[] buffer;
        }

        else if(luettu.compare("varit")==0) {
            int koko;
            file >> koko;
            
            file.seekg(1, file.cur); //eliminoi rivinvaihto
            int bytes = koko * sizeof(float);
            float* buffer = new float[koko];
            file.read((char*)buffer, bytes);
            
            varit.insert(varit.end(),buffer,buffer+koko);
            
            delete[] buffer;
        }
    
        //Jos lisättiin joku vaihe, on pisteitä varmasti tullut lisää. Lisätään vaiheet-vektoriin:
        if(vaiheet.size() != pisteet.size()) {
            vaiheet.resize(pisteet.size(), vaihe);
        }
        
        //Jos vaihe on Kalibroi, lisätään väreiksi kalibraation väriä
        if(vaihe == Kalibroi) {
            varit.resize(pisteet.size(), kalibraationVari);
        }
    } 
    file.close();
    
    result.muodostaViiva(pisteet, paineet, paksuudet, sumeudet, vaiheet, varit, nimi);
    cout << "ladattiin pisteet: " << result.pisteet.size() << "\n"; 
    
    return result;
}


void tiedosto::tallennaViiva(Viiva viiva, std::string polku) {
}

