#include "Monitori.h"
#include "tilastot.h"

void pensseli::setup() {
    brushFbo.allocate(MAX_KOKO, MAX_KOKO, GL_RGBA);
    brushFbo.begin();
        ofClear(clearColor);
    brushFbo.end();
    ofEnableAlphaBlending();
}


void pensseli::drawBrush() {       
    const int blur_steps = 10;
    //float koko_step = koko / (2*blur_steps);
    ofPoint P(MAX_KOKO/2, MAX_KOKO/2);
    
    brushFbo.begin();
        ofClear(clearColor);
        
        //piirretään valkoisella brush läpinäkyvälle. Stroken piirrossa käytetään värimodulointia
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        int alpha = 2.6 * 256 / blur_steps;
        ofSetColor(255,255,255, alpha);
        
        for(unsigned int blur_i = 0; blur_i < blur_steps; blur_i++) {
            float suhde = (float)blur_i / (float)blur_steps;
            float r = pow(suhde, blur) * (koko * (1 + blur/3) ); //maksimisäde on (2+1/3) * koko. Maksimihalkaisija on siis (2+1/3) * 2 * koko. Joten: koko <= MAX_KOKO/(4+2/3)
            ofDrawCircle(P, r);
        }
        
    brushFbo.end();
}


void pensseli::strokeTo(ofPoint kohde) {
    //jos viivaa ei ole aloitettu, aloitetaan se nyt eikä vielä piirretä
    if(!viivaJatkuu) {
        sijainti = kohde;
        viivaJatkuu = true;
        return;
    }
    
    if(sijainti == kohde) {
        return;
    }
    
    //päivitä uusi pensseli parametrien mukaan
    drawBrush();
    
    //tee viiva nykyisestä sijainnista kohteeseen
    ofVec2f v = kohde - sijainti;
    float L = v.length();
    
    //tehdään v:stä suuntavektori
    v.normalize();
    
    //piirrä tasavälisesti brushia viivalle:
    if(koko < 1) koko = 1;
    float step = koko * spacing;    
    ofVec2f brushMiddle(-MAX_KOKO/2, -MAX_KOKO/2);
    
    //blendausasetukset fbo:oon piirtämistä varten
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
    for(float l = 0; l < L; l += step)  {
        ofSetColor(vari);
        brushFbo.draw(sijainti + v*l + brushMiddle);
    }
    
    //päivitetään sijainti
    sijainti = kohde;
}


void pensseli::moveTo(ofPoint kohde) {
    sijainti = kohde;
}


void pensseli::lopetaViiva() {
    viivaJatkuu = false;
    //nyt jos pyydetään piirtämään, täytyy siirtää ensin alkupää paikalleen
}


void Monitori::setup() {
    pensseli::setup();
    //alusta viivaFbo
    viivaFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    viivaFbo.begin();
        ofClear(pensseli::clearColor);
    viivaFbo.end();
    
}


void Monitori::draw() {
    ofClear(taustaVari);
    
    ofSetColor(255,255,255, pow(viivanAlfa, 0.7) * 255);
    
    viivaFbo.draw(0,0);
    
    if(fadeOut && viivanAlfa > 0) {
        viivanAlfa -= 0.025;
        if(viivanAlfa <0) {
            viivanAlfa = 0;
            fadeOut = false;
        }
    }
    else if(fadeIn && viivanAlfa < 1) {
        viivanAlfa += 0.015;
        if(viivanAlfa > 1) {
            viivanAlfa = 1;
            fadeIn = false;
        }
    }
}


void Monitori::piirraViiva(const Viiva& viiva) {
    if(viiva.pisteet.empty() ) {
        return;
    }    

    //sumeus on 0...1
    float sumeus = viiva.sumeus.arvot.back();
    //pehmennetään ottamalla 8 viimeistä arvoa
    //float sumeus = keskiarvo(viiva.haeArvot(&viiva.sumeus, 8) );
    
    //jos sumeus on täysi, ei piirretä mitään
 /*   if(sumeus == 1) {
        pensseli::moveTo(viiva.pisteet.back().sijainti);
        return;
    }    
   */ 
    //paksuus on 0...1
    //pehmennetään ottamalla 8 viimeistä arvoa
    float paksuus = viiva.paksuus.keskiarvot.back();
    // blur: 0...16
    pensseli::blur = ofClamp(pow(sumeus, 2) * 16, 0.1, 16);
    
    // koko: 0 ... MAX_KOKO/(4+2/3)
    pensseli::koko = ofClamp(pow(paksuus, 0.7) * MAX_KOKO / (4+2/3), 1, MAX_KOKO / (4+2/3) );    
    
    viivaFbo.begin();
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        pensseli::strokeTo( viiva.pisteet.back());
    viivaFbo.end();
}


void Monitori::piirraVari(ofColor vari_) {
    //taustaa piirretään koko ajan, joten tämä riittää:
    taustaVari = vari_;
    pensseli::vari = vari_;
}


void Monitori::tyhjenna() {
    viivaFbo.begin();
        ofClear(pensseli::clearColor);
    viivaFbo.end();
    pensseli::lopetaViiva();
}


void Monitori::paljasta() {
    fadeOut = false;
    fadeIn = true;
    showing = true;
}


void Monitori::piilota() {
    fadeOut  = true;
    fadeIn = false;
    showing = false;
}


void Monitori::tallennaKuvana(std::string tiedosto) {
    ofPixels px;
    viivaFbo.readToPixels(px);
    ofSaveImage(px, tiedosto);
}