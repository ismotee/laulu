#include "Kyna.h"

void Kyna::setup() {
    if (!hidpen::setup("./hidpenSettings") )
        cout << "Kynän setup ei onnistunut\n";
    else
        cout << "Kynän setup onnistui\n";

    paine = 0;
    paikka = ofPoint();
    paikka_scaled = ofPoint();
    drag = false;
}

void Kyna::update() {
    click = false;
    if(hidpen::isOpen) {
        hidpen::update();
        if(hidpen::pressure >= 0 && hidpen::pressure <= 1
           && hidpen::x > 0 && hidpen::x <= 1
           && hidpen::y > 0 && hidpen::y <= 1
        )
        {
            paine = hidpen::pressure;
            paikka = ofPoint(hidpen::x * ofGetWidth(), (hidpen::y) * ofGetHeight() );
            paikka_scaled = ofPoint(hidpen::x, 1-hidpen::y);
        
            if(hidpen::pressure > 0) {
                if(!drag) {
                    click = true;
                    drag = true;
                }
            }
            else {
                click = false;
                drag = false;
            }
        }
        else {
            //tuli huono paketti! Ei päivitetä mitään
            #ifdef HIDPEN_DEBUG 
                    std::cout << "Kyna: huono paketti: (" << hidpen::x << ", " << hidpen::y << ", " << hidpen::pressure << ")\n";
            #endif
            return;
        }
    }
}

void Kyna::draw() {
    ofSetColor(ofColor::white);
    ofDrawCircle(paikka, 4);
}

void Kyna::asetaPaikka(float x, float y) {
    //deprekoitu! Käytä vain updatea
}

void Kyna::pressed(float x, float y) {
    //asetaPaikka(x, y);    
    click = true;
    drag = true;
}

void Kyna::moved(float x, float y) {
    //asetaPaikka(x, y);
}

void Kyna::released(float x, float y) {
    //asetaPaikka(x, y);
    //click = false;
    //drag = false;
}

void Kyna::kerroTiedot() {
    std::cout << "paikka: (" << paikka.x << ", " << paikka.y << ") / (" << paikka_scaled.x << ", " << paikka_scaled.y << ")\n";
    std::cout << "click: " << (click? "true" : "false") << "\n";
}

