#ifndef LYMATH_IOPS_H
#define LYMATH_IOPS_H

namespace iops {
	int clampi(int v, int min, int max);
	int gmini(int v, int min);
	int gmaxi(int v, int min);
}

namespace lymath {
	using namespace iops;
}

#endif //LYMATH_IOPS_H
