#include "iops.h"

namespace iops {
	int clampi(int v, int min, int max) {
		if (v < min) { v = min; } if (v > max) { v = max; }
		return v;
	}

	int gmini(int v1, int v2) {
		if (v1 < v2) { return v1; } return v2;
	}

	int gmaxi(int v1, int v2) {
		if (v1 > v2) { return v1; } return v2;
	}
}
