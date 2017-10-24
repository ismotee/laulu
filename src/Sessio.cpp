#include "Sessio.h"

void Sessio::setup(int viivaKeskenCount_) {
    viivaKeskenCount = viivaKeskenCount_;
    updateCount = 0;
}

void Sessio::update(int nappi) // valitaan moodi joka kierroksella
{

    if (nappi == 1) {
        moodi = piirtaa;
        updateCount = 0;
    } else if(moodi!= viivaValmis){
        updateCount++;
    }

    if (moodi == piirtaa  && updateCount > viivaKeskenCount) {
        moodi = valmistelee;
    } else if(moodi == valmistelee) {
        moodi = viivaValmis;
        updateCount = 0;
    }

}

