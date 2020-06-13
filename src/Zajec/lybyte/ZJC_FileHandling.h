#ifndef __ZJC_FileHandling_H__
#define __ZJC_FileHandling_H__

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

#include "../ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

int    writecrk   (cchar* filename, cchar* archive, char* offset, char* mode);

#ifdef __cplusplus
}
#endif

#endif //__ZJC_FileHandling_H__
