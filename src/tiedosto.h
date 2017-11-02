#pragma once
#include "Viiva.h"


namespace tiedosto {
  
    Viiva lataaViiva(std::string tiedostonNimi);
    bool tallennaViiva(const Viiva& viiva, std::string polku);
};