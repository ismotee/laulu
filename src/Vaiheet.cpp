#include "Vaiheet.h"

//{Kulje,Kalibroi,Improvisoi,LaskeKohde,LahestyKohdetta,Viimeistele,Keskeyta,EiOle};

VaiheetEnum tulkkaaVaihe(std::string vaiheStr) {
    //tassa kartassa on vaiheiden kutsumanimet:
    static const std::map<std::string, VaiheetEnum> vaiheSanat {
        { "kalibrointi", Kalibroi },
        { "Improvisoi", Improvisoi },
        { "LahestyKohdetta", LahestyKohdetta }
    };

    //katsotaan kartasta, vastaako annettu sana jotain vaihetta, ja palautetaan se vaihe
    const auto iter = vaiheSanat.find( vaiheStr );
    if( iter != vaiheSanat.cend() )
        return iter->second;
    
    //jos ei ymmarretty sanaa, palautetaan vaihe 'EiOle'
    return EiOle;
}

std::string vaiheenNimi(VaiheetEnum vaihe) {
    switch(vaihe) {
        case Kalibroi: return "kalibrointi";
        case Improvisoi: return "Improvisoi";
        case LahestyKohdetta: return "LahestyKohdetta";
        default: return "EiOle";
    }
}

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
