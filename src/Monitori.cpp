#include "Monitori.h"
#include "tilastot.h"

void smooth::add(float newValue) {
    if(values.size() == max_size) {
        values[add_i] = newValue;
        add_i++;
        if(add_i >= max_size) 
            add_i = 0;
    }
    else if (values.size() < max_size) {
        values.push_back(newValue);
    }
    else if (values.size() > max_size)
        values.resize(max_size);
}


float smooth::get() {
    return keskiarvo(values);
}


ofColor pensseli::clearColor = ofColor(0, 0, 0, 0);


void pensseli::setup() {
    brushFbo.allocate(MAX_KOKO, MAX_KOKO, GL_RGBA);
    pensseli::clearColor = ofColor(0,0,0,0);
    brushFbo.begin();
        ofClear(clearColor);
    brushFbo.end();
    ofEnableAlphaBlending();
}


void pensseli::drawBrush() {       
    const int blur_steps = 20; //oli: 10
    //float koko_step = koko / (2*blur_steps);
    ofPoint P(MAX_KOKO/2, MAX_KOKO/2);
    
    brushFbo.begin();
        ofClear(ofColor(200,200,200, 0));
        
        //piirretään valkoisella brush läpinäkyvälle. Stroken piirrossa käytetään värimodulointia
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        int alpha = 2.9 * 256 / blur_steps; //oli: 2.6
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
    float MAX_STROKE_LENGTH = 1000;
    if(L > MAX_STROKE_LENGTH) {
        std::cout << "pensseli::strokeTo: liian pitkä veto! (" << L << ")\n";
        return;
    }
    
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
    pensseli::clearColor = ofColor(0,0,0,0);
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


void Monitori::teeVeto(ofPoint kohde, float paksuus, float sumeus) {
    //jos sumeus on täysi, ei piirretä mitään
    if(sumeus == 1) {
        pensseli::moveTo(kohde);
        return;
    }
    // koko: 0 ... MAX_KOKO/(4+2/3)
    pensseli::koko = ofClamp(pow(paksuus, 0.7) * MAX_KOKO / (4+2/3), 1, MAX_KOKO / (4+2/3) ); 
    // blur: 0...16
    pensseli::blur = ofClamp(pow(sumeus, 2) * 16, 0.1, 16);    
    
    viivaFbo.begin();
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        pensseli::strokeTo( kohde );
    viivaFbo.end();    
}


void Monitori::piirraViiva(const Viiva& viiva) {
    if(viiva.pisteet.empty() ) {
        return;
    }    

    //paksuus on 0...1
    //pehmennetään ottamalla 6 viimeistä arvoa
    float paksuus = keskiarvo(viiva.paksuus.arvot, viiva.paksuus.tarkistaKoko(6) );

    //sumeus on 0...1
    //otetaan viimeisin arvo
    float sumeus = viiva.sumeus.back();
    
    ofPoint P = viiva.pisteet.back();
    
    //jos sumeus on täysi, ei piirretä mitään
    if(sumeus == 1) {
        pensseli::moveTo(P);
        return;
    }    
    
    teeVeto(P, paksuus, sumeus);
}


void Monitori::piirraKokoViiva(const Viiva& viiva) {
    if(viiva.pisteet.empty() ) {
        return;
    }
    
    smooth paksuusSmooth;
    
    //tyhjennetään ja piirretään eka väri
    tyhjenna();
    piirraVari( viiva.varit[0] );
    
    //std::vector<float> paksuudet = viiva.haeArvot(&viiva.paksuus);
    //std::vector<float> sumeudet = viiva.haeArvot(&viiva.sumeus);
    
    for(unsigned int i=0; i<viiva.size(); i++) {
        //paksuus on 0...1
        paksuusSmooth.add(viiva.paksuus.arvot[i] );
        float paksuus = paksuusSmooth.get();
        float sumeus = viiva.sumeus[i];
        teeVeto(viiva.pisteet[i], paksuus, sumeus);
    }
}


void Monitori::piirraViivaAlusta(const Viiva& viiva, unsigned int n) {
    if(viiva.empty() ) {
        return;
    }
    
    if(n >= viiva.size() ) {
        piirraKokoViiva(viiva);
        return;
    }
    
    //käytetään tätä pehmentämiseen:
    smooth paksuusSmooth;
    
    //tyhjennä ja piirrä eka väri
    tyhjenna();
    piirraVari(viiva.varit[0] );
    
    for(unsigned int i=0; i<n; i++) {        
        //paksuus on 0...1
        paksuusSmooth.add(viiva.paksuus.arvot[i] );
        float paksuus = paksuusSmooth.get();
        float sumeus = viiva.sumeus[i];
        teeVeto(viiva.pisteet[i], paksuus, sumeus);
    }
}


void Monitori::piirraVari(ofColor vari_) {
    //taustaa piirretään koko ajan, joten tämä riittää:
    taustaVari = vari_;
    pensseli::vari = vari_;
}


void Monitori::tyhjenna() {
    pensseli::clearColor = ofColor(0,0,0,0);
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


void Monitori::piirraKartta(const std::vector<Viiva>& viivat, float r) {
    if(viivat.empty() ) {
        std::cerr << "Monitori::piirraKartta: Ei viivoja!\n";
        return;
    }

    viivaFbo.begin();

    //piirretään viivat karttaan
    for(unsigned int i=0; i<viivat.size(); i++) {        
/*        float x = viivat[i].paksuus.keskiarvot.back() * ofGetWidth();
        float y = (1 - viivat[i].sumeus.keskiarvot.back() ) * ofGetHeight();
        ofSetColor(viivat[i].varit[i]);*/
        float x = viivat[i].kalibraatio.paksuusKa * ofGetWidth();
        float y = (1 - viivat[i].kalibraatio.sumeusKa) * ofGetHeight();
        ofSetColor(viivat[i].kalibraatio.vari);
        
        ofDrawCircle(x,y,r);
    }

    viivaFbo.end();
}



void Monitori::tallennaKartta(const std::vector<Viiva>& viivat, std::string filename) {
    if(viivat.empty() ) {
        std::cerr << "Monitori::tallennaKartta: Ei viivoja!\n";
        return;
    }
    
    //luodaan kartta omaan fbo:oon
    ofFbo karttaFbo;
    int w = ofGetWidth();
    int h = ofGetHeight();
    karttaFbo.allocate(w, h, GL_RGBA);
    karttaFbo.begin();
    //taustavärinä sama kuin blurrin reunat
    ofClear(pensseli::clearColor);
    
    std::cerr << "kartan koko: " << w << " x " << h << "\n";
    std::cout << "max: " << GL_MAX_FRAMEBUFFER_WIDTH << " x " << GL_MAX_FRAMEBUFFER_HEIGHT << "\n";
    
    //piirretään viivat karttaan
    for(unsigned int i=0; i<viivat.size(); i++) {
/*        
        float x = viivat[i].haeViimeisinPaksuus().keskiarvo * w;
        float y = (1 - viivat[i].haeViimeisinSumeus().keskiarvo) * h;
        std::cerr << "piirretään karttaan (" << x << ", " << y << ")\n";
        
        ofSetColor(viivat[i].vari);
        ofDrawCircle(x,y,10);
 */
    }
    
    karttaFbo.end();
    
    ofPixels px;
    karttaFbo.readToPixels(px);
    ofSaveImage(px, filename);
}


void Multimonitori::teeVeto(ofPoint kohde, unsigned int pensseli_i, float paksuus, float sumeus, ofColor vari) {
    if(pensseli_i >= pensselit.size() ) {
        std::cout << "Multimonitori::teeVeto: ei pensseliä " << pensseli_i << " (" << pensselit.size() << ")\n";
        return;
    }
    
    kohde *= viivaScale;
    
    //jos sumeus on täysi, ei piirretä mitään
    if(sumeus == 1) {
        pensselit[pensseli_i].moveTo(kohde);
        return;
    }
    // koko: 0 ... MAX_KOKO/(4+2/3)
    pensselit[pensseli_i].koko = ofClamp(pow(paksuus, 0.7) * pensseli::MAX_KOKO / (4+2/3), 1, pensseli::MAX_KOKO / (4+2/3) );    
    // blur: 0...16
    pensselit[pensseli_i].blur = ofClamp(pow(sumeus, 2) * 16, 0.1, 16);
    
    pensselit[pensseli_i].vari = vari;
    
    viivaFbo.begin();
        pensselit[pensseli_i].strokeTo( kohde );
    viivaFbo.end();
}


void Multimonitori::setup(unsigned int pensseli_n) {

    //jos pensselien määrä on annettu, luodaan ne; muuten jätetään tyhjäksi ja luodaan tarvittaessa
    if(pensseli_n > 0) {
        pensselit.resize(pensseli_n);
        for(pensseli& p : pensselit) {
            p.setup();
        }
    }
    
    //alusta viivaFbo
    viivaFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    viivaFbo.begin();
        ofClear(pensseli::clearColor);
    viivaFbo.end();    
}


void Multimonitori::luoPensselit(unsigned int n) {
    pensselit.resize(n);
    for(pensseli& p : pensselit)
        p.setup();
    
    //tyhjenna();
    lopetaViivat();
}


void Multimonitori::draw() {
    //ofClear(taustaVari);
    ofSetColor(ofColor::white);
    viivaFbo.draw(0,0);
}


void Multimonitori::piirraViivatAlusta(const std::vector<Viiva>& viivat, unsigned int n) {
    tyhjenna();
    
    if(viivat.empty() ) {
        return;
    }
        
    //luodaan pensselit jos niitä ei ole
    if(pensselit.size() < viivat.size() ) {
        luoPensselit(viivat.size() );
    }
        
    std::vector<smooth> paksuusSmooths(viivat.size() );
    
    //piirretään n vetoa:
    for(unsigned int i=0; i<n; i++) {
        //piirretään joka viivasta yksi veto:
        for(unsigned int viiva_i = 0; viiva_i < viivat.size(); viiva_i++) {
            if(i >= viivat[viiva_i].size() )
                continue;
            
            float sumeus = viivat[viiva_i].sumeus[i];

            //pehmennys ottamalla 8 viimeistä arvoa
            paksuusSmooths[viiva_i].add(viivat[viiva_i].paksuus[i] );
            float paksuus = paksuusSmooths[viiva_i].get();
            
            //käytetään pensseli-indeksinä viivan indeksiä viiva_i:
            teeVeto( viivat[viiva_i].pisteet[i], viiva_i, paksuus, sumeus, viivat[viiva_i].varit[i] );
        }
    }
}


void Multimonitori::piirraViivatKohdasta(const std::vector<Viiva>& viivat, unsigned int n) {
    if(viivat.empty() ) {
        return;
    }
        
    //luodaan pensselit jos niitä ei ole
    if(pensselit.size() < viivat.size() ) {
        luoPensselit(viivat.size() );
    }
    
    for(unsigned int viiva_i = 0; viiva_i < viivat.size(); viiva_i++) {
        if(n >= viivat[viiva_i].size() )
            continue;

        //pehmennys ottamalla 10 viimeistä arvoa
        int paksuusSampleSize;
        if(n < 10)
            paksuusSampleSize = n;
        else
            paksuusSampleSize = 10;
        
        //kopioidaan paksuuden arvoista otanta pehmennystä varten
        std::vector<float> viimePaksuudet(viivat[viiva_i].paksuus.arvot.begin() + n - paksuusSampleSize, viivat[viiva_i].paksuus.arvot.begin() + n);
        float paksuus = keskiarvo(viimePaksuudet);
        
        float sumeus = viivat[viiva_i].sumeus[n];
        
        //käytetään pensseli-indeksinä viivan indeksiä viiva_i:
        teeVeto( viivat[viiva_i].pisteet[n], viiva_i, paksuus, sumeus, viivat[viiva_i].varit[n] );
    }
}

void Multimonitori::piirraVari(ofColor vari_) {
    //taustaa piirretään koko ajan, joten tämä riittää:
    viivaFbo.begin();
    ofClear(vari_);
    viivaFbo.end();
   // pensseli::vari = vari_;
}


void Multimonitori::tallennaKuvana(std::string filename) {
    ofPixels px;
    viivaFbo.readToPixels(px);
    ofSaveImage(px, filename);
}

void Multimonitori::tyhjenna() {
    pensseli::clearColor = ofColor(0,0,0,255);
    viivaFbo.begin();
        ofClear(pensseli::clearColor);
    viivaFbo.end();
    lopetaViivat();
}


void Multimonitori::lopetaViivat() {
    for(pensseli& p : pensselit)
        p.lopetaViiva();
}

void Multimonitori::piirraKartta(const std::vector<Viiva>& viivat, float r) {
    if(viivat.empty() ) {
        std::cerr << "Monitori::piirraKartta: Ei viivoja!\n";
        return;
    }

    viivaFbo.begin();

    //piirretään viivat karttaan
    for(unsigned int i=0; i<viivat.size(); i++) {        
/*        float x = viivat[i].paksuus.keskiarvot.back() * ofGetWidth();
        float y = (1 - viivat[i].sumeus.keskiarvot.back() ) * ofGetHeight();
        ofSetColor(viivat[i].varit[i]);*/
        float x = viivat[i].kalibraatio.paksuusKa * ofGetWidth();
        float y = (1 - viivat[i].kalibraatio.sumeusKa) * ofGetHeight();
        ofSetColor(viivat[i].kalibraatio.vari);
        
        ofDrawCircle(x,y,r);
    }

    viivaFbo.end();
}

void Multimonitori::tallennaKartta(const std::vector<Viiva>& viivat, std::string filename) {
    if(viivat.empty() ) {
        std::cerr << "Monitori::tallennaKartta: Ei viivoja!\n";
        return;
    }
    
    //luodaan kartta omaan fbo:oon
    ofFbo karttaFbo;
    int w = ofGetWidth();
    int h = ofGetHeight();
    karttaFbo.allocate(w, h, GL_RGBA);
    karttaFbo.begin();
    //taustavärinä sama kuin blurrin reunat
    ofClear(pensseli::clearColor);
    
    std::cerr << "kartan koko: " << w << " x " << h << "\n";
    std::cout << "max: " << GL_MAX_FRAMEBUFFER_WIDTH << " x " << GL_MAX_FRAMEBUFFER_HEIGHT << "\n";
    
    //piirretään viivat karttaan
    for(unsigned int i=0; i<viivat.size(); i++) {
/*        
        float x = viivat[i].haeViimeisinPaksuus().keskiarvo * w;
        float y = (1 - viivat[i].haeViimeisinSumeus().keskiarvo) * h;
        std::cerr << "piirretään karttaan (" << x << ", " << y << ")\n";
        
        ofSetColor(viivat[i].vari);
        ofDrawCircle(x,y,10);
 */
    }
    
    karttaFbo.end();
    
    ofPixels px;
    karttaFbo.readToPixels(px);
    ofSaveImage(px, filename);
}

int Multimonitori::suurimmanKoko(const std::vector<Viiva>& viivat) {
    if(viivat.empty()) 
        return 0;
    int suurin = viivat[0].size();
    for(int i=0; i<viivat.size(); i++ ) {
        if(viivat[i].size() > suurin)
            suurin = viivat[i].size();        
    }    
    return suurin;
}

void Multimonitori::piirraViivatLopusta(const std::vector<Viiva>& viivat) {
    int suurin = suurimmanKoko(viivat);
    if(suurin <= 0) 
        return;
    
    piirraViivatKohdasta(viivat, suurin);    
}

void Multimonitori::piirraViivatKokonaan(const std::vector<Viiva>& viivat) {
    int suurin = suurimmanKoko(viivat);
    if(suurin <= 0) 
        return;
    
    piirraViivatAlusta(viivat, suurin);
}



void Multimonitori::piirraPiste(ofVec2f p) {
    viivaFbo.begin();
        ofSetColor(ofColor::black);
        ofDrawCircle(p.x * ofGetWidth(), (1-p.y) * ofGetHeight(), 15);
        ofSetColor(ofColor::white);
        ofDrawCircle(p.x * ofGetWidth(), (1-p.y) * ofGetHeight(), 10);
    viivaFbo.end();
}