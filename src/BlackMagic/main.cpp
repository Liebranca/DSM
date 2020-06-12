//  THIS FILE HANDLES CONVERSION TO DSM FORMATS
//  WE READ FROM HEXDUMPS CREATED FROM BLENDER PYTHON

//  - --- - --- - --- - --- -

#include <cstdlib>
#include <cstdio>

#include "lyutils/ZJC_Evil.h"
#include "lybyte/ZJC_FileHandling.h"

//const strvec dsm_extensions = {"crk", "joj", "ans"};

//  - --- - --- - --- - --- -

int main(int argc, char* argv[])
{

    if(argc != 5) { fprintf(stderr,
        "usage: %s <filename> <archive> offset mode ", argv[0]); exit(EXIT_FAILURE); }

    int errorcode = 0;
    WARD_EVIL_WRAP(errorcode, zjc::writecrk(argv[1], argv[2], argv[3], argv[4]));

    return 0;
}
