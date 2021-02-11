#ifndef __ZAJEC_STRING_H__
#define __ZAJEC_STRING_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../ZJC_CommonTypes.h"

//  ---     ---     ---     ---     ---
/* so Visual Studio __apparently__ won't let me add a path shorthand as macro        |
|  not without a property sheet, which fucks with the *.vcxproj files in dumb ways   |
|  no way I'm taking that route. let's just def it like a knobhead, eh?             */

#define MKSTR(X) __MSTR__(X)
#define __MSTR__(X) #X
#define CAT(X,Y) __CAT__(X,Y)
#define __CAT__(X,Y) X##Y

#define MKPATH(root, file) MKSTR(CAT(root, file))

//  ---     ---     ---     ---     ---
/* put roots in defines here, eg C:/Module/include without quotes (leave that to MKSTR) |
|  empty for now but you never know. this might come in handy in a few years           */

//      - --- - --- - --- - --- -

char* subpath(cchar* fullpath, uint levels);
void  catpath(cchar* root, cchar* added, char** dst);

//      - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZAJEC_STRING_H__
