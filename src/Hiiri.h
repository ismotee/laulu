#pragma once
#include "ofMain.h"

struct Hiiri {
    ofPoint mouseState;
    bool click;
    virtual void mouseMoved(float x,float y) {mouseState = ofPoint(x,y,mouseState.z);}
    virtual void mouseReleased(float x,float y){mouseState = ofPoint(x,y,0);}
    virtual void mousePressed(float x, float y){mouseState = ofPoint(x,y,1);click = true;}
    virtual void mouseDragged(float x, float y) {mouseState = ofPoint(x,y,mouseState.z);}
};