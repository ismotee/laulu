#include "Vaiheet.h"

void Vaiheet::setup() {
    vaiheetEnum = Kulje;
    Kyna::setup();
}

void Vaiheet::update() {
    
    switch(vaiheetEnum) {
        case Kulje:
            vaiheetEnum = kulje();
            break;
        case Kalibroi:
            vaiheetEnum = kalibroi();
            break;
        case Improvisoi:
            vaiheetEnum = improvisoi();
            break;
        case LaskeKohde:
            vaiheetEnum = laskeKohde();
            break;
        case LahestyKohdetta:
            vaiheetEnum = lahestyKohdetta();
            break;
        case Viimeistele:
            vaiheetEnum = viimeistele();
            break;
        case Keskeyta:
            vaiheetEnum = keskeyta();
            break;
        default:
            vaiheetEnum = keskeyta();
            
            break;
        
    }
    
    //kyna::update vasta lopuksi koska muuten click boolean tyhjenee ennen kuin sitä on luettu
    Kyna::update();
}

void Vaiheet::verbose() {
    //cout << vaiheetEnum << "\n";
    static VaiheetEnum viimeVaihe = Kulje;
    if(vaiheetEnum != viimeVaihe) {        
        cout << toString() << "\n";
        viimeVaihe = vaiheetEnum;
    }
}

std::string Vaiheet::toString() {
    switch(vaiheetEnum) {
        case Kulje: return "kulje";
        case Kalibroi: return "kalibroi";
        case Improvisoi: return "improvisoi";
        case LaskeKohde: return "laskeKohde";
        case LahestyKohdetta: return "lahestyKohdetta";
        case Viimeistele: return "viimeistele";
        case Keskeyta: return "keskeyta";
    }
}
