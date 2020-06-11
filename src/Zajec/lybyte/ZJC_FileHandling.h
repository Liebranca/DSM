#ifndef __ZJC_FileHandling_H__
#define __ZJC_FileHandling_H__

//  - --- - --- - --- - --- -

#include "../ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

extern "C" namespace zjc {

    static  FILE*       curfile;
    static  FILE*       tempfile;

    int    openbin    (cchar* filename, cchar* mode, cchar* loc, bool shutit = false);
    int    closebin   (cchar* filename, cchar* loc, bool shutit = false);
    int    writecrk   (cchar* filename, cchar* archive, char* offset, char* mode, cchar* loc);

}

#endif //__ZJC_FileHandling_H__
