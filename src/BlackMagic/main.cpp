//  THIS FILE HANDLES CONVERSION TO DSM FORMATS
//  WE READ FROM HEXDUMPS CREATED FROM BLENDER PYTHON

//  - --- - --- - --- - --- -

#include <iostream>

#include "lyutils/evil.h"
#include "lybyte/files.h"

//const strvec dsm_extensions = {"crk", "joj", "ans"};

//  - --- - --- - --- - --- -

int main(int argc, char* argv[])
{   
    zjc::writecrk("D:\\lieb_git\\dsm\\src\\BlackMagic\\Cube.crk", GETLOC);
    
    return 0;
}
