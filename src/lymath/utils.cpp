#include <iostream>
#include <math.h>
#include <fstream>
#include <iterator>

#include "utils.h"

glm::vec3 ZVEC3 = { 0,0,0 };

namespace lymath {

	bool approxf(float v1, float v2, float m) {
		return v2 - m < v1 && v1 < v2 + m;
	}

	float clampf(float v, float min, float max) {
		if (v < min) { v = min; } if (v > max) { v = max; }
		return v;
	}

	float ilerpf(float i1, float i2, float l, float p) {
		float d = fabs(i1 - i2) / l;
		int s = 1; if (i1 > i2) { s = -1; }
		return i1 + d * (p * s);
	}

	float absclampf(float v, float min, float max) {
		int dirn = 1; if (v < 0) { dirn = -1; }
		if (fabs(v) < min) { v = min; } if (fabs(v) > max) { v = max; }
		return v * dirn;
	}

	float gmaxf(float v1, float v2) {
		if (v1 > v2) { return v1; } return v2;
	}

	float gminf(float v1, float v2) {
		if (v1 < v2) { return v1; } return v2;
	}

	float submaxf(float v1, float v2) {
		return gmaxf(v1, v2) - gminf(v1, v2);
	}

	float compolarf(float v1, float v2) {
		if ((v1 < 0 && v2 > 0) || (v1 > 0 && v2 < 0)) { return -v1; } return v1;
	}

	int clampi(int v, int min, int max) {
		if (v < min) { v = min; } if (v > max) { v = max; }
		return v;
	}

	int gmini(int v1, int v2) {
		if (v1 < v2) { return v1; } return v2;
	}

	float planedist(float d1, float d2, float x, float y, float z) {
		return fabs((d2 * d2) - d1) / gminf(sqrt((x * x) + (y * y) + (z * z)), 0.025f);
	}

	float pointdist(float p1x, float p1y, float p1z,
		float p2x, float p2y, float p2z) {
		return sqrt(pow(p2x - p1x, 2) + pow(p2y - p1y, 2) + pow(p2z - p1z, 2));
	}

	float codist(float x1, float x2) {
		return sqrt(pow(x2 - x1, 2));
	}

	glm::vec3 axisdist(float p1x, float p1y, float p1z,
		float p2x, float p2y, float p2z) {
		return { codist(p1x, p2x), codist(p1y, p2y), codist(p1z, p2z) };
	}

	glm::vec3 xzvec(glm::vec3 v) {
		return { v.x, 0, v.z };
	}

	float vec3len(glm::vec3 v) {
		return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	}

	unsigned int vec3min(glm::vec3 v) {
		unsigned int index = 0; float value;
		for (unsigned int i = 0; i < 3; i++) {
			if (i == 0) { value = v[i]; }
			else { if (v[i] < value) { value = v[i]; index = i; } }
		} std::cout << index << std::endl;  return index;
	}

	void vec3inverti(glm::vec3 v, unsigned int i) {
		v[i] = -v[i];
	}

	void printVec2(glm::vec2 v) {
		std::cout << v[0] << ", " << v[1] << std::endl;
	}

	void printVec3(glm::vec3 v) {
		std::cout << v[0] << ", " << v[1] << ", " << v[2] << std::endl;
	}

	void printVec4(glm::vec4 v) {
		std::cout << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << std::endl;
	}

};

namespace lybyte {
	
	std::vector<bool> sumbytes(std::vector<std::vector<bool>> bytes) {
		std::vector<bool> result;
		for (unsigned int i = 0; i < bytes.size(); i++) {
			for (unsigned int j = 0; j < bytes[i].size(); j++) {
				result.push_back(bytes[i][j]);
			}
		}

		return result;
	}

	std::vector<bool> cfile_sec(unsigned int iStart, unsigned int iEnd) {
		std::vector<std::vector<bool>> bytearray;
		for (unsigned int i = iStart; i < iEnd; i++) {
			bytearray.push_back(tobits(cfile[i], 8));
		}
		return sumbytes(bytearray);
	}

	std::vector<bool> takebits(std::vector<bool> byte, unsigned int iStart, unsigned int iEnd) {
		std::vector<bool> bits;
		for (unsigned int i = iStart; i < iEnd; i++) {
			bits.push_back(byte[i]);
		}
		return bits;
	}

	std::vector<bool> tobits(unsigned int x, unsigned int bytesize) {
		x = lymath::gmini(x, (int)pow(2, bytesize) - 1);
		std::vector<bool> byte;

		for (unsigned int i = 0; i < bytesize; i++) {
			byte.push_back(x % 2);
			x = x / 2;
		}

		return byte;
	}

	std::vector<unsigned char> frombin(const std::string& filename) {
		std::ifstream input(filename, std::ios::binary); std::vector<unsigned char> output;
		if (!input) { std::cerr << "Unable to open file " << filename << std::endl; exit(1); }
		cfile = std::vector<unsigned char>(std::istreambuf_iterator<char>(input), {});
		input.close();

		return cfile;
	}

	std::string cpath_fromreg(unsigned int obid, const std::string& regpath) {
		unsigned int regid = unsigned int(obid / 256);

		std::ifstream file; std::string line;
		std::string reg = regpath + "reg" + std::to_string(regid) + ".irf";
		file.open(reg.c_str());

		if (!file) { std::cerr << "Unable to open file " << reg << std::endl; exit(1); }

		unsigned int i = 0; std::string found;
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
	
	unsigned int todecimal(std::vector<bool> byte) {
		unsigned int x = 0;
		for (unsigned int i = 0; i < byte.size(); i++) {
			x += byte[i] * (int)pow(2, i);
		}

		return x;
	}

	unsigned int cuint2(unsigned int i) {		
		return todecimal(cfile_sec(i, i + 2));
	}
	unsigned int cuint3(unsigned int i) {
		return todecimal(cfile_sec(i, i + 3));
	}

	float cfloat2(unsigned int i) {
		std::vector<bool> b1 = tobits(cfile[i], 8), b2 = tobits(cfile[i + 1], 8);
		
		bool sign = b1[0]; 
		
		std::vector<bool> floatbits = takebits(b1, 1, 8);
		std::vector<bool> zerobits = { b2[0], b2[1] };
		std::vector<bool> intbits = takebits(b2, 2, 8);

		int ivalue = todecimal(intbits); float fvalue = (float)todecimal(floatbits) * 0.0078125f;

		if (zerobits[0] || zerobits[1]) {
			if (zerobits[0] && !zerobits[1]) {
				fvalue -= 0.001f;
			}
			else if (!zerobits[0] && zerobits[1]) {
				fvalue += 0.001f;
			}
			else if (zerobits[0] && zerobits[1]) {
				fvalue *= 0.1f;
			}
		}

		float result = ivalue + fvalue;
		if (sign) { result *= -1; }

		return result;
	}

	void cfile_close() { cfile.clear(); }

};

namespace lyarr {

	int findis(std::string arr[], const std::string& val) {
		for (unsigned int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++) {
			if (arr[i] == val) { return i; }
		} return -1;
	}

	void prints(std::string arr[], std::string separator, std::string prefix = "*.") {
		for (unsigned int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++) {
			std::cout << prefix + arr[i] + separator;
		}
	}

}
