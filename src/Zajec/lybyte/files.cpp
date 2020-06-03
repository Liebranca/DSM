#include <iostream>
#include <fstream>
#include "files.h"

namespace files {

//	- --- - --- - --- - --- -

	void frombin(const std::string& filename) {
		std::ifstream input(filename, std::ios::binary);
		if (!input) { std::cerr << "Unable to open file " << filename << std::endl; exit(1); }
		curfile = binfile(std::istreambuf_iterator<char>(input), {});
		input.close();
	}

//	- --- - --- - --- - --- -
	
	byte sumbytes(bytearray arr) {
		/*byte b;
		for (uint i = 0; i < arr.size(); i++) {
			for (uint j = 0; j < arr[i].size(); j++) {
				b.push_back(arr[i][j]);
			}
		}

		return b;*/
		return 0x00;
	}

//	- --- - --- - --- - --- -

	byte curfile_sec(uint iStart, uint iEnd) {
		/*bytearray arr;
		for (uint i = iStart; i < iEnd; i++) {
			arr.push_back(types::tobits(curfile[i], 8));
		}
		return sumbytes(arr);*/

		return 0x00;
	}

//	- --- - --- - --- - --- -

	std::string cpath_fromreg(uint obid, const std::string& regpath) {
		uint regid = uint(obid / 256);

		std::ifstream file; std::string line;
		std::string reg = regpath + "reg" + std::to_string(regid) + ".irf";
		file.open(reg.c_str());

		if (!file) { std::cerr << "Unable to open file " << reg << std::endl; exit(1); }

		uint i = 0; std::string found;
		while (std::getline(file, line))
		{
			if (i + regid * 256 == obid) {
				found = line; break;
			}
			i++;
		}
		file.close();
		return found;
	}

//	- --- - --- - --- - --- -

	uint cuint2(uint i) { types::touint(curfile_sec(i, i + 2)); }
	uint cuint3(uint i) { types::touint(curfile_sec(i, i + 3)); }

//	- --- - --- - --- - --- -

	float cfloat2(uint i) {
		/*return types::tofloat2( types::tobits(curfile[i], 8),
							  types::tobits(curfile[i + 1], 8) );*/
		return 0;
	}

//	- --- - --- - --- - --- -

	void cfile_close() { curfile.clear(); }

}