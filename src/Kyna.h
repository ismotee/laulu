#pragma once
#include "ofMain.h"
#include "hidpen.h"

struct Kyna {
    
    ofPoint paikka;//paikka ruudun pikseleinä
    ofPoint paikka_scaled; //skaalattuna [0..1] niin että vasen alakulma on (0,0)
    float paine;
    bool click, drag;
    
    void setup();
    void update();
    void draw();
    
    void asetaPaikka(float x, float y);
    void pressed(float x, float y); 
    void moved(float x, float y);
    void released(float x, float y);
    void kerroTiedot();
        
};
