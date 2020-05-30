#ifndef LYBYTE_FILES_H
#define LYBYTE_FILES_H

//	· --- · --- · --- · --- ·

#include <string>
#include "types.h"

//	· --- · --- · --- · --- ·

namespace files {
	static binfile curfile;
	void frombin(const std::string& filename);

	byte sumbytes(bytearray arr);
	byte curfile_sec(uint iStart, uint iEnd);

	uint cuint2(uint i);
	uint cuint3(uint i);
	float cfloat2(uint i);	

	std::string cpath_fromreg(uint obid, const std::string& regpath);
	void cfile_close();
}

//	· --- · --- · --- · --- ·

namespace lybyte {
	using namespace files;
}

#endif //LYBYTE_FILES_H
