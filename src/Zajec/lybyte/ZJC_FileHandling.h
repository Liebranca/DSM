#ifndef __ZJC_FileHandling_H__
#define __ZJC_FileHandling_H__

//  - --- - --- - --- - --- -

#include "../ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

extern "C" namespace zjc {

    int    writecrk   (cchar* filename, cchar* archive, char* offset, char* mode);

}

#endif //__ZJC_FileHandling_H__
