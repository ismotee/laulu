#include "Kyna.h"

void Kyna::setup() {
    if (!hidpen::setup("./hidpenSettings") )
        cout << "Kynän setup ei onnistunut\n";
    else
        cout << "Kynän setup onnistui\n";

    paine = 0;
    paikka = ofPoint();
    paikka_scaled = ofPoint();
}

void Kyna::update() {
    click = false;

    if(hidpen::isOpen) {
        hidpen::update();
        paine = hidpen::pressure;
    }
}

void Kyna::asetaPaikka(float x, float y) {
    paikka = ofPoint(x,y);    
    paikka_scaled = ofPoint(x / ofGetWidth(), 1 - (y / ofGetHeight() ) );
}

void Kyna::pressed(float x, float y) {
    asetaPaikka(x, y);    
    click = true;
    drag = true;
}

void Kyna::moved(float x, float y) {
    asetaPaikka(x, y);
}

void Kyna::released(float x, float y) {
    asetaPaikka(x, y);
    click = false;
    drag = false;
}

void Kyna::kerroTiedot() {
    std::cout << "paikka: (" << paikka.x << ", " << paikka.y << ") / (" << paikka_scaled.x << ", " << paikka_scaled.y << ")\n";
    std::cout << "click: " << (click? "true" : "false") << "\n";
}

