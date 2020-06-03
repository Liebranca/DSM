#ifndef LYBYTE_FILES_H
#define LYBYTE_FILES_H

//  - --- - --- - --- - --- -

#include <string>
#include "types.h"

//  - --- - --- - --- - --- -

namespace files {
    static binfile curfile;

    // Copies contents from dump into curfile
    void frombin(const std::string& filename);

    //  Reinterpret bytearray as a single byte, ie flatten it.
    byte sumbytes(bytearray arr);

    // Returns a sub-array from curfile[iStart-iEnd]
    byte curfile_sec(uint iStart, uint iEnd);

    // Takes byte from curfile at position i and interprets as uint2
    uint cuint2(uint i);

    // Takes byte from curfile at position i and interprets as uint3
    uint cuint3(uint i);

    // Takes byte from curfile at position i and interprets as float2
    float cfloat2(uint i);  

    // Gets filepath from *.irf textfiles
    std::string cpath_fromreg(uint obid, const std::string& regpath);
    void cfile_close();
}

//  - --- - --- - --- - --- -

namespace lybyte {
    using namespace files;
}

#endif //LYBYTE_FILES_H
