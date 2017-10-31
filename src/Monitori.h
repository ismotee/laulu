#include "Viiva.h"

class smooth{
    int add_i = 0;
public:
    std::vector<float> values;
    float get();
    void add(float);
    unsigned int max_size = 10;
};


class pensseli{
public:
    static const int MAX_KOKO = 300;
    
    bool viivaJatkuu = false;
    
    float koko = 10;
    float blur = 0.1;
    float spacing = 0.2; // suhteena pensselin koosta. huom: vaikuttaa sumennukseen olennaisesti! oli 0.4
    static ofColor clearColor;
    ofColor vari = ofColor::lightPink;

    ofPoint sijainti;
    
    ofFbo brushFbo; //tähän piirretään yksi pensselin piste
    
    void setup(); // luo fbo:n

    void drawBrush(); // päivittää brushFbo:n
    void strokeTo(ofPoint kohde);
    void moveTo(ofPoint kohde);
    void lopetaViiva();
};


class Monitori : public pensseli {
public:
    static ofVec2f paksuusSumeusHiiresta();

    ofColor taustaVari = ofColor::black;
    ofFbo viivaFbo;
    
    float viivanAlfa = 0;
    bool fadeIn = false;
    bool fadeOut = false;
    bool showing = false;
    
    void setup();
    void draw();
    
    void teeVeto(ofPoint kohde, float paksuus, float sumeus);
    
    void piirraViiva(const Viiva&);
    void piirraKokoViiva(const Viiva&);
    void piirraViivaAlusta(const Viiva&, unsigned int n);
    void piirraVari(ofColor vari_);
    
    void piirraKartta(const std::vector<Viiva>& viivat, float r = 10);
    
    void tyhjenna();
    
    void paljasta();
    void piilota();
    
    void tallennaKuvana(std::string filename = "kuvat/default.png");
    void tallennaKartta(const std::vector<Viiva>& viivat, std::string filename = "kuvat/kartta.png");
};


class Multimonitori {

public:
    std::vector<pensseli> pensselit;
    ofFbo viivaFbo;

    ofColor taustaVari = ofColor::black;
    
    void teeVeto(ofPoint kohde, unsigned int pensseli_i, float paksuus, float sumeus, ofColor vari);

    void setup(unsigned int pensseli_n = 0);
    void luoPensselit(unsigned int n);
    void draw();

    void piirraViivatAlusta(const std::vector<Viiva>& viivat, unsigned int n);
    void piirraViivatKohdasta(const std::vector<Viiva>& viivat, unsigned int n);

    void tyhjenna();
    void lopetaViivat();
    void tallennaKuvana(std::string filename = "kuvat/default.png");
};