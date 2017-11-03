#include "tiedosto.h"
#include <iterator>
#include <iostream>
#include <time.h>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <algorithm>


int getMonthAsInt(const std::string & monthName) {
    static const std::map<std::string, int> months {
        { "Jan", 1 },
        { "Feb", 2 },
        { "Mar", 3 },
        { "Apr", 4 },
        { "May", 5 },
        { "Jun", 6 },
        { "Jul", 7 },
        { "Aug", 8 },
        { "Sep", 9 },
        { "Oct", 10 },
        { "Nov", 11 },
        { "Dec", 12 }
    };
    const auto iter = months.find( monthName );
    if( iter != months.cend() )
        return iter->second;
    return 0;
}


std::string convertTimeFormat(std::string oldTime) {
    // vanha muoto on: Thu Aug 17 19:15:44 2017.ov
    // uusi muoto on: 2017-08-17_19-15-44
    std::stringstream ss(oldTime);
    string weekday;
    string monthName;
    int day;
    int hour;
    int min;
    int second;
    int year;

    char c;    
    ss >> weekday >> monthName >> day >> hour >> c >> min >> c >> second >> year;
    
    int month = getMonthAsInt(monthName);

    char stringBuffer[20];
    int n = sprintf(stringBuffer, "%04d-%02d-%02d_%02d-%02d-%02d", year, month, day, hour, min, second);
    return std::string(stringBuffer);
}


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
                //luetaan väri, jos ollaan kalibroinnissa
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
            float* buffer = new float[2*koko];
            file.read((char*)buffer, bytes);
            
            //tallenna pisteet vektoriin
            for(int piste_i = 0; piste_i < koko; piste_i++) {
                float x = buffer[piste_i*2];
                float y = buffer[piste_i*2 + 1];
                pisteet[alkuKoko + piste_i] = ofPoint(x,y);
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

        else if(luettu.compare("värit")==0) {
            int koko; //montako väriä
            file >> koko;
            
            //tehdään tilaa vektoriin
            int alkuKoko = varit.size();
            varit.resize(varit.size()+koko);
            
            //aletaan lukea värejä. Väri on 3 char (r, g, b, r, ...)
            file.seekg(1, file.cur); //eliminoi rivinvaihto
            int bytes = koko * 3 * sizeof(char);
            char* buffer = new char[koko*3];
            file.read((char*)buffer, bytes);
            
            //tallenna värit vektoriin
            for(int vari_i = 0; vari_i < koko; vari_i++) {
                float r = buffer[vari_i*3];
                float g = buffer[vari_i*3 + 1];
                float b = buffer[vari_i*3 + 2];
                varit[alkuKoko + vari_i] = ofColor(r,g,b);
            }
            
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


bool tiedosto::tallennaViiva(const Viiva& viiva, std::string polku) {
    std::string filename = polku + viiva.nimi + ".ov";
    std::ofstream file (filename, std::ofstream::binary);
    
    std::string versionumero = std::to_string(Viiva::versio);
    std::string sarja("sarjan_nimi_tähän");
    std::string viivanKoko = std::to_string(viiva.size() );

    if(file.is_open() ) {
        //kirjoitetaan koko jutun header:
        std::string headerStr(
                viiva.nimi + '\n'
                + versionumero + '\n'
                + sarja + '\n'
        );                
        file.write(headerStr.c_str(), headerStr.length() );
                
        std::string vaiheHeaderStr;        
        std::string blockHeaderStr;

        //Käydään läpi viivan pisteitä. Kun vaihe vaihtuu, kirjoitetaan blokki
        
        int readPos = 0; //mihin asti on luettu, indeksinä viivan pisteisiin
        int writePos = 0; //mihin asti on viimeksi kirjoitettu
        
        VaiheetEnum viimeVaihe = Kalibroi;
        while(readPos < viiva.size() ) {
            for(; viiva.vaiheet[readPos] == viimeVaihe; readPos++) {
            }
            //readPos osoittaa nyt uuden vaiheen ensimmäiseen kohtaan viivassa
            //kirjoitetaan viime kirjoituksen lopusta kohtaan readPos
            int blockSize = readPos - writePos;

            //bufferit joihin laitetaan uuden vaiheen tiedot
            float* pisteBuffer = new float[blockSize * 2];//tallennetaan koordinaatit peräkkäin: x, y, x, y, ...
            float* paineBuffer = new float[blockSize];
            float* paksuusBuffer = new float[blockSize];
            float* sumeusBuffer = new float[blockSize];
            char* varitBuffer = new char[blockSize*3]; //r, g, b, r, g, b, ...

            //kopioidaan tiedot viivasta, alkaen viime kirjoituksen lopusta, päättyen kohtaan writePos+blockSize
            //p_i: monesko vaiheen piste
            for(int p_i = 0; p_i < blockSize; p_i++) {
                pisteBuffer[p_i*2] = viiva.pisteet[writePos + p_i].x;
                pisteBuffer[p_i*2+1] = viiva.pisteet[writePos + p_i].y;
            }        
            std::copy(viiva.paine.arvot.begin() + writePos, viiva.paine.arvot.begin() + writePos + blockSize, paineBuffer);
            std::copy(viiva.paksuus.arvot.begin() + writePos, viiva.paksuus.arvot.begin() + writePos + blockSize, paksuusBuffer);
            std::copy(viiva.sumeus.arvot.begin() + writePos, viiva.sumeus.arvot.begin() + writePos + blockSize, sumeusBuffer);
            
            for(int v_i = 0; v_i < blockSize; v_i++) {
                varitBuffer[v_i*3]   = viiva.varit[writePos + v_i].r;
                varitBuffer[v_i*3+1] = viiva.varit[writePos + v_i].g;
                varitBuffer[v_i*3+2] = viiva.varit[writePos + v_i].b;
            }        
            
            vaiheHeaderStr = std::string(
                    "vaihe\n"
                    + vaiheenNimi(viimeVaihe) + "\n"
            );
            if(viimeVaihe == Kalibroi) {
                std::string variStr(
                    "väri " 
                    + std::to_string(viiva.kalibraatio.vari.r) + " "
                    + std::to_string(viiva.kalibraatio.vari.g) + " "
                    + std::to_string(viiva.kalibraatio.vari.b) 
                );
                vaiheHeaderStr += variStr + "\n";
            }
            
            file.write(vaiheHeaderStr.c_str(), vaiheHeaderStr.length() );
            
            blockHeaderStr = std::string("pisteet\n" + std::to_string(blockSize) + "\n");
            file.write(blockHeaderStr.c_str(), blockHeaderStr.length() );
            file.write( (char*) pisteBuffer, sizeof(float) * 2 * blockSize);

            blockHeaderStr = std::string("paineet\n" + std::to_string(blockSize) + "\n");
            file.write(blockHeaderStr.c_str(), blockHeaderStr.length() );
            file.write( (char*) paineBuffer, sizeof(float) * blockSize);
            
            blockHeaderStr = std::string("paksuudet\n" + std::to_string(blockSize) + "\n");
            file.write(blockHeaderStr.c_str(), blockHeaderStr.length() );
            file.write( (char*) paksuusBuffer, sizeof(float) * blockSize);
            
            blockHeaderStr = std::string("sumeudet\n" + std::to_string(blockSize) + "\n");
            file.write(blockHeaderStr.c_str(), blockHeaderStr.length() );
            file.write( (char*) sumeusBuffer, sizeof(float) * blockSize);

            if(viiva.vaiheet[readPos] != Kalibroi) {
                blockHeaderStr = std::string("värit\n" + std::to_string(blockSize) + "\n");
                file.write(blockHeaderStr.c_str(), blockHeaderStr.length() );
                file.write( (char*) varitBuffer, sizeof(char) * blockSize * 3);
            }
            
            writePos += blockSize;
            viimeVaihe = viiva.vaiheet[readPos];
            std::cout << "blokki luettu: (" << readPos << " / " << writePos << ")\n";
            
            delete[] pisteBuffer;
            delete[] paineBuffer;
            delete[] paksuusBuffer;
            delete[] sumeusBuffer;
            delete[] varitBuffer;
        }
        file.close();
        std::cout << "tallennettiin: " << filename << "\n";
        return true;
    }
    else {
        std::cout << "ei voitu tallentaa: " << filename << "\n";
        return false;
    }
}

