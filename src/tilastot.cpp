#include "tilastot.h"
#include <numeric>
#include <cmath>


float keskiarvo(const std::vector<float>& v){
    if(v.empty() ) return 0;
    
    float summa = std::accumulate(v.begin(), v.end(), 0.0);
    return summa / v.size();
}


float keskiarvo(const std::vector<float>& v, int otanta){
    if(v.empty() ) return 0;
    
    float summa = std::accumulate(v.begin()+(v.size()-otanta), v.end(), 0.0);
    return summa / otanta;
}


float varianssi(const std::vector<float>& v) {
    if(v.size() < 2)
        return 0;
    float sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
    float mean = keskiarvo(v);
    return sq_sum / v.size() - mean * mean;
}


float keskihajonta(const std::vector<float>& v) {
    return std::sqrt(varianssi(v) );
}



float varianssi(const std::vector<float>& v, int otanta) {
    if(v.size() < 2)
        return 0;
    float sq_sum = std::inner_product(v.begin()+(v.size()-otanta), v.end(), v.begin()+(v.size()-otanta), 0.0);
    float mean = keskiarvo(v,otanta);
    return sq_sum / otanta - mean * mean;
}


float keskihajonta(const std::vector<float>& v, int otanta) {
    return std::sqrt(varianssi(v,otanta));
}


