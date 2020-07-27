#include "ZJC_Str.h"
#include "ZJC_Evil.h"

#include <string.h>

//      - --- - --- - --- - --- -

cchar* subpath(cchar* path, uint levels) {

    char*  file    = (char*)path;
    cchar* curchar = '\0';
    uint   len     = strlen(path);
    uint   curlev  = 0;

    for(uint i = len; i > 2; i--)
    {

        curchar = &path[i];

        if ((*curchar == '\\') || (*curchar == '/'))
        {
            curlev++;
            if(curlev == levels)                { file[i] = '\0'; break;                                                }

        }
    }

    return (cchar*)file;                                                                                                }

//      - --- - --- - --- - --- -

cchar* catpath(cchar* root, cchar* added)       {

    uint  rootlen  = strlen(root);
    uint  addedlen = strlen(added);

    char* catted = (char*) evil_malloc(rootlen + addedlen, sizeof(char));

    strcat(catted, root);
    catted[rootlen] = '\0';

    strcat(catted, added);
    catted[rootlen + addedlen] = '\0';

    return (cchar*)catted;                                                                                              }

