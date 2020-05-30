#ifndef LYMATH_FOPS_H
#define LYMATH_FOPS_H

//	- --- - --- - --- - --- -

namespace fops {
	
	// Find if v1 is at an m minimum distance from v2
	bool approxf(float v1, float v2, float m = 0.010f);

	// Clamp float v to range(min, max) inclusive
	float clampf(float v, float min, float max);

	// Slower clampf that accounts for negative range
	//* if v < 0: range(-min, -max)
	//* if v >= 0: range(min, max)
	float absclampf(float v, float min, float max);

	// Drunkly-written linear interpolation
	float ilerpf(float i1, float i2, float l, float p);

	// Get max(v1, v2)
	float gmaxf(float v1, float v2);

	// Get min(v1, v2)
	float gminf(float v1, float v2);

	// Get (max(v1, v2) - min(v1, v2))
	float submaxf(float v1, float v2);

	// Invert v1 if it's opposite polarity from v2
	float compolarf(float v1, float v2);

	// Flat one-dimentional distance
	float codistf(float x1, float x2);
}

//	- --- - --- - --- - --- -

namespace lymath {
	using namespace fops;
}

#endif //LYMATH_FOPS_H
