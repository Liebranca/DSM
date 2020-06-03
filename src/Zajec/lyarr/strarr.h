#ifndef LYARR_STRARR_H
#define LYARR_STRARR_H

#include <string>
#include <vector>

typedef std::vector<std::string> strvec;

//  - --- - --- - --- - --- -

namespace strarr {
    // Return index of element in array, -1 if not
    int findstr(strvec vec, std::string val);

    // Same as findstr, without a care for the index
    bool hasstr(strvec vec, std::string val);

    // Prints elements of string array
    void printstr(strvec vec, std::string separator = ", ", std::string prefix = "*.");

}

//  - --- - --- - --- - --- -

namespace lyarr {
    using namespace strarr;
}

#endif //LYARR_STRARR_H
