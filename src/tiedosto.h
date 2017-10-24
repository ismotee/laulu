#pragma once
#include "Viiva.h"


namespace tiedosto {
  
    Viiva lataaViiva(std::string tiedostonNimi);
    void tallennaViiva(Viiva viiva, std::string polku);
    std::string aika();
};