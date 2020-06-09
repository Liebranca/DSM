#include <iostream>
#include <string>
#include <memory>

#include "types.h"
#include "../lymath/gops.h"
#include "../lyutils/evil.h"

namespace zjc {

    zjc::fvRange frac16_range(128, 0.0078125f, "");

//  - --- - --- - --- - --- -

    int takebits(byte b1, uint iStart, uint iEnd)   {
        
        int x = 0b0;
        for (uint i = iStart; i < iEnd; i++)        { x += (b1[i]<<i);  }
                                                      return x;         }

//  - --- - --- - --- - --- -

    float frac16tofloat(byte b1, byte b2)           {

        bool sign = b1[0]; int floatbits = takebits(b1, 1, 8) >> 1;
        int zerobits = takebits(b2, 0, 2); int intbits = takebits(b2, 2, 8) >> 2;
        
        float fvalue = (float)floatbits * 0.0078125f;

//      - --- - --- - --- - --- -

        if (zerobits)                               {
            if      (zerobits == 1)                 { fvalue -= 0.001f; }
            else if (zerobits == 2)                 { fvalue += 0.001f; }
            else if (zerobits == 3)                 { fvalue *= 0.1f;   }
                                                                        }

//      - --- - --- - --- - --- -

        float result = intbits + fvalue;
        if (sign)                                   { result *= -1;     }

        return result;
    }

//      - --- - --- - --- - --- -

    frac16::frac16(float v) : b1(0b0), b2(0b0) {
        if (abs(v) >= 64)
        { std::cerr << v << " is out of FRAC16 range (63.992188)" << std::endl; std::exit(1); }

        bool sign = 0; if (v < 0) { sign = 1; } v = abs(v);
        std::string s = std::to_string(v); size_t fromdot = s.rfind(".");

        std::string decimals =  s.substr(fromdot+1, s.npos);
        std::string integer =   s.substr(0, fromdot);
    
        size_t closest = 0; int rounding = 0b00;

        if (decimals.at(0) == '0') {
            if (decimals.at(1) != '0') { 
                closest = frac16_range.take_closest(
                    std::stof("0."+decimals) * 10);

                rounding = 0b11;}
                                                                                    }

        else {  float w = std::stof("0."+decimals);
                closest = frac16_range.take_closest(w);
        
                if ( frac16_range[closest] > w )        { rounding = 0b01; }
                else if ( frac16_range[closest] < w )   { rounding = 0b10; }
                                                                                    }
    
        this->b1 = (closest << 1) + sign;
        this->b2 = (std::stoi(integer) << 2) + rounding;

    }
}
