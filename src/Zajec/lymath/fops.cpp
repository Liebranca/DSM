#include <math.h>
#include "fops.h"

//	- --- - --- - --- - --- -

namespace fops {

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

//	- --- - --- - --- - --- -

	float gmaxf(float v1, float v2) {
		if (v1 > v2) { return v1; } return v2;
	}

	float gminf(float v1, float v2) {
		if (v1 < v2) { return v1; } return v2;
	}

	float submaxf(float v1, float v2) {
		return gmaxf(v1, v2) - gminf(v1, v2);
	}

//	- --- - --- - --- - --- -

	float compolarf(float v1, float v2) {
		if ((v1 < 0 && v2 > 0) || (v1 > 0 && v2 < 0)) { return -v1; } return v1;
	}

	float codistf(float x1, float x2) {
		return sqrt(pow(x2 - x1, 2));
	}

}