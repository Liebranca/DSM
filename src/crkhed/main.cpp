//--- --- --- --- --- --- --- --- ---
//	THIS FILE HANDLES CONVERSION TO DSM FORMATS
//	WE READ FROM HEXDUMPS CREATED FROM BLENDER PYTHON

//--- --- --- --- --- --- --- --- ---
//	SETUP
//--- --- --- --- --- --- --- --- ---
//		0.	LIBRARIES
//	--- 	--- 	--- 	--- 	---

#include <iostream>
#include <string>
#include <vector>

#include "../lymath/utils.h"

//		0.	CONTAINERS
//	--- 	--- 	--- 	--- 	---

//static container for valid extension names
std::string dsm_extensions[] = {"crk", "joj", "ans"};

//		666.	DONE	.999 || ->DS
//--- END CRK BLOCK --- --- --- --- --- --- --- ---


//--- --- --- --- --- --- --- --- ---
//	CRK BLOCK
//--- --- --- --- --- --- --- --- ---

// crkhed entry point
//* never call this from dsm
//* meant to be used from blender
int main(int argc, char* argv[])
{

//		0.	HANDLE ARGS
//
//		argv == {crkhed, filepath}
//	--- 	--- 	--- 	--- 	---
	std::string filepath, extension = "";
	if (argc > 1) {
		filepath = argv[1];
		size_t fromdot = filepath.rfind(".");
		if (fromdot != std::string::npos) {
			extension = filepath.substr(fromdot+1, filepath.npos);
		}

		int index = lyarr::findis(dsm_extensions, extension);
		if (index != -1) {
			if		(extension == "crk") { ; } //call crk converter
			else if (extension == "joj") { ; } //call joj converter
			else if (extension == "ans") { ; } //call ans converter
		}
		else { 
			std::cout << "invalid file extension. accepted files are: ";
			lyarr::prints(dsm_extensions);
		}

	}
	else { std::cout << "you need to provide arguments for this call, asshole."; }

	return 0;
}
