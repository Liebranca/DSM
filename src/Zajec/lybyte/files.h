#ifndef LYBYTE_FILES_H
#define LYBYTE_FILES_H

//  - --- - --- - --- - --- -

#include "../constdefs.h"

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
    
    static  FILE*       curfile;

    void    openbin    (concha* filename, concha* mode, concha* loc);
    void    closebin   (concha* filename, concha* loc);
    void    writecrk   (concha* filename, concha* loc);

}

#endif //LYBYTE_FILES_H
