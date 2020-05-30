#ifndef LYUTILS_H
#define LYUTILS_H

#include <string>
#include <vector>
#include "glm/glm.hpp"

extern glm::vec3 ZVEC3;


namespace lymath {

	struct bf {
		bool a;
		float x;
	};
	
	bool approxf(float v1, float v2, float m = 0.010f);
	float clampf(float v, float min, float max);	
	float absclampf(float v, float min, float max);
	float ilerpf(float i1, float i2, float l, float p);
	float gmaxf(float v1, float v2);
	float gminf(float v1, float v2);
	float submaxf(float v1, float v2);
	float compolarf(float v1, float v2);
	
	int clampi(int v, int min, int max);
	int gmini(int v, int min);

	float planedist(float d1, float d2, float x, float y, float z);
	float pointdist(float p1x, float p1y, float p1z,
		float p2x, float p2y, float p2z);
	float codist(float x1, float x2);
	glm::vec3 axisdist(float p1x, float p1y, float p1z,
		float p2x, float p2y, float p2z);

	glm::vec3 xzvec(glm::vec3 v);
	float vec3len(glm::vec3 v);
	unsigned int vec3min(glm::vec3 v);
	void vec3inverti(glm::vec3 v, unsigned int i);

	void printVec2(glm::vec2 v);
	void printVec3(glm::vec3 v);
	void printVec4(glm::vec4 v);

};

namespace lybyte {

	static std::vector<unsigned char> cfile;
	std::vector<unsigned char> frombin(const std::string& filename);
	std::vector<bool> sumbytes(std::vector<std::vector<bool>> bytes);
	std::vector<bool> cfile_sec(unsigned int iStart, unsigned int iEnd);
	std::vector<bool> takebits(std::vector<bool> byte, unsigned int iStart, unsigned int iEnd);
	std::vector<bool> tobits(unsigned int x, unsigned int bytesize);
	std::string cpath_fromreg(unsigned int obid, const std::string& regpath);

	unsigned int todecimal(std::vector<bool> byte);
	unsigned int cuint2(unsigned int i);
	unsigned int cuint3(unsigned int i);

	float cfloat2(unsigned int i);
	void cfile_close();

};

#endif // LYUTILS_H