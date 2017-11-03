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
}

void Kyna::draw() {
    ofSetColor(ofColor::gray);
    ofDrawCircle(paikka, 2);
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

