#include <iostream>
#include "..\Zajec.h"
#include "strarr.h"

namespace strarr {

//  - --- - --- - --- - --- -

    int findstr(strvec vec, std::string val) {
        for (uint i = 0; i < vec.size(); i++) {
            if (vec[i] == val) { return i; }
        } return -1;
    }

//  - --- - --- - --- - --- -

    bool hasstr(strvec vec, std::string val) {
        return findstr(vec, val) != -1;
    }

//  - --- - --- - --- - --- -

    void printstr(strvec vec, std::string separator, std::string prefix) {
        for (uint i = 0; i < vec.size(); i++) {
            std::cout << prefix + vec[i];
            if (i < vec.size() - 1) { std::cout << separator; }

        } std::cout << "\n";
    }

}