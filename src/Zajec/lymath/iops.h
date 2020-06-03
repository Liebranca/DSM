#ifndef LYMATH_IOPS_H
#define LYMATH_IOPS_H

//  - --- - --- - --- - --- -

namespace iops {
    // Clamp int to range(min, max)
    int clampi(int v, int min, int max);

    // Get min(v1, v2)
    int gmini(int v1, int v2);

    // Get max(v1, v2)
    int gmaxi(int v1, int v2);
}

//  - --- - --- - --- - --- -

namespace lymath {
    using namespace iops;
}

#endif //LYMATH_IOPS_H
