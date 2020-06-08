#ifndef LYBYTE_FILES_H
#define LYBYTE_FILES_H

//  - --- - --- - --- - --- -

#include "types.h"

//  - --- - --- - --- - --- -

constexpr char crk_sign[16] = { 0x46, 0x43, 0x4b, 0x42,
                                0x21, 0x54, 0x43, 0x48,
                                0x45, 0x53, 0x47, 0x45,
                                0x54, 0x24, 0x24, 0x24 };

struct CrkFile {
    const char magik[16] = { 0x46, 0x43, 0x4b, 0x42,
                             0x21, 0x54, 0x43, 0x48,
                             0x45, 0x53, 0x47, 0x45,
                             0x54, 0x24, 0x24, 0x24 };

    std::uint16_t numVerts, numInds;

};

//  - --- - --- - --- - --- -

namespace zjc {
    
    void    openbin        (const char filename[]);
    void    writecrk   (const char filename[]);

}

#endif //LYBYTE_FILES_H
