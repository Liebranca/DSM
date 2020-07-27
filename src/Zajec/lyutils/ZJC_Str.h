#ifndef __ZAJEC_STRING_H__
#define __ZAJEC_STRING_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../ZJC_CommonTypes.h"

//      - --- - --- - --- - --- -

cchar* subpath(cchar* fullpath, uint levels);
cchar* catpath(cchar* root, cchar* added);

//      - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZAJEC_STRING_H__
