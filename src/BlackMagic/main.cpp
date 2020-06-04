//  THIS FILE HANDLES CONVERSION TO DSM FORMATS
//  WE READ FROM HEXDUMPS CREATED FROM BLENDER PYTHON

//  - --- - --- - --- - --- -

#include <iostream>
#include "lyarr/strarr.h"
#include "lybyte/types.h"

#include "Zajec.h"

//static container for valid extension names
const strvec dsm_extensions = {"crk", "joj", "ans"};

//  - --- - --- - --- - --- -

// crkhed entry point
//* never call this from dsm
//* meant to be used from blender
int main(int argc, char* argv[])
{
//  argv == {crkhed, filepath}
    
    /*std::string filepath, extension = "";
    if (argc > 1) {
        filepath = argv[1];
        size_t fromdot = filepath.rfind(".");
        if (fromdot != std::string::npos) {
            extension = filepath.substr(fromdot+1, filepath.npos);
        }

// - -- - --- - --- - --- - --- -

        if (lyarr::hasstr(dsm_extensions, extension)) {
            if      (extension == "crk") { ; } //call crk converter
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
