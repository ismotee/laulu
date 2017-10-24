#pragma once
#include "ofMain.h"
#include "hidpen.h"

struct Kyna {
    
    ofPoint paikka;
    float paine;
    bool click, drag;
    
    void setup();

    void update();
    
    void pressed(float x, float y); 
    void moved(float x, float y);
    void released(float x, float y);
    
    
};
