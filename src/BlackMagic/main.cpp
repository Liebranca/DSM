//  THIS FILE HANDLES CONVERSION TO DSM FORMATS
//  WE READ FROM HEXDUMPS CREATED FROM BLENDER PYTHON

//  - --- - --- - --- - --- -

#include <iostream>

//#include "lyarr/strarr.h"
#include "lyarr/dynarray.h"
#include "lyutils/evil.h"

//const strvec dsm_extensions = {"crk", "joj", "ans"};

//  - --- - --- - --- - --- -

int main(int argc, char* argv[])
{   
    $ERRLOC;
    zjc::fArray(2, ERRLOC);

    //lybyte::writecrk("D:\\lieb_git\\dsm\\src\\BlackMagic\\Cube.crk");
    /*std::string filepath, extension = "";
    if (argc > 1) {
        filepath = argv[1];
        size_t fromdot = filepath.rfind(".");
        if (fromdot != std::string::npos) {
            extension = filepath.substr(fromdot+1, filepath.npos);
        }

// - -- - --- - --- - --- - --- -

        if (lyarr::hasstr(dsm_extensions, extension)) {
            if      (extension == "crk") { lybyte::writecrk(filepath.c_str()); }
            else if (extension == "joj") { ; } //call joj converter
            else if (extension == "ans") { ; } //call ans converter
        }
        else { 
            std::cout << "Invalid file extension. accepted files are: ";
            lyarr::printstr(dsm_extensions);
        }

    }

//  - --- - --- - --- - --- -

    else { std::cout << "You need to provide arguments for this call.\n"; }*/
    return 0;
}
