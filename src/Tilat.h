#pragma once
#include "ofMain.h"

enum Tila {Rajaa, Piirtaa, Soittaa};

struct Tilat {
    Tila tila;
    
    void update() {
        if(tila == Rajaa)
            rajaa();
        else if(tila == Soittaa)
            soittaa();
        else
            piirtaa();
    }
    
    std::string toString() {
        if(tila == Rajaa)
            return "rajaa";
        if(tila == Soittaa)
            return "soittaa";
        
        return "piirtaa";
        
    }
    
    void vaihdaTilaa(){
        if(tila==Rajaa)
            tila=Piirtaa;
        else if(tila==Piirtaa)
            tila=Soittaa;
        else
            tila=Rajaa;
    }
    
    
    virtual void rajaa() = 0;
    virtual void piirtaa() = 0;
    virtual void soittaa()= 0;
    
};
