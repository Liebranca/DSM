#ifndef LYMATH_FOPS_H
#define LYMATH_FOPS_H

namespace fops {
	bool approxf(float v1, float v2, float m = 0.010f);
	float clampf(float v, float min, float max);
	float absclampf(float v, float min, float max);
	float ilerpf(float i1, float i2, float l, float p);
	float gmaxf(float v1, float v2);
	float gminf(float v1, float v2);
	float submaxf(float v1, float v2);
	float compolarf(float v1, float v2);
	float codistf(float x1, float x2);
}

namespace lymath {
	using namespace fops;
}

#endif //LYMATH_FOPS_H
