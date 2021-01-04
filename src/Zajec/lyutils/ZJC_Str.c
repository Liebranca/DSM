#include "ZJC_Str.h"
#include "ZJC_Evil.h"

#include <stdio.h>
#include <string.h>

//      - --- - --- - --- - --- -

char* subpath(cchar* path, uint levels) {

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

    return file;                                                                                                        }

//      - --- - --- - --- - --- -

void catpath(cchar* root,
             cchar* added,
             char** dst  )                      {

    uint  rootlen  = strlen                     (root                                                                   );
    uint  addedlen = strlen                     (added                                                                  );

    WARD_EVIL_MALLOC                            (*dst, char, sizeof(char), rootlen + addedlen + 2                       );

    strcat                                      (*dst, root                                                             );
    strcat                                      (*dst, added                                                            );
                                                                                                                        }

