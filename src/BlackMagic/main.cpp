//  THIS FILE HANDLES CONVERSION TO DSM FORMATS
//  WE READ FROM HEXDUMPS CREATED FROM BLENDER PYTHON

//  - --- - --- - --- - --- -

#include <iostream>
#include <string>

#include "lyutils/ZJC_Evil.h"
#include "lybyte/ZJC_FileHandling.h"

//const strvec dsm_extensions = {"crk", "joj", "ans"};

//  - --- - --- - --- - --- -

int main(int argc, char* argv[])
{   
    zjc::writecrk(argv[1], argv[2], argv[3], argv[4], GETLOC);
    return 0;
}
