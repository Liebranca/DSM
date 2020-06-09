#ifndef ZAJEC_EVIL_H
#define ZAJEC_EVIL_H

//  - --- - --- - --- - --- -

#include "../unsigdefs.h"
#include "../constdefs.h"

//  - --- - --- - --- - --- -

namespace zjc {

    void    terminate(int errorcode, concha* loc, concha* info = "");

    char*   evil_shpath(concha* f);

    void*   evil_malloc(size_t count, size_t size, concha* loc);
    void    evil_free(void* buff);

    concha* evil_geterrloc(concha* file, concha* a, concha* func, concha* b, concha* line);

}



//  - --- - --- - --- - --- -

#define WARD_EVIL_MALLOC(x, y) if (x == nullptr) { zjc::terminate(0x00, y); }
#define WARD_EVIL_FOPEN(x, y, z) if (!x) { zjc::terminate(0x02, y, z); }
#define WARD_EVIL_FCLOSE(x, y, z) if (x != 0) { zjc::terminate(0x03, y, z); }
#define WARD_EVIL_FWRITE(x, y, z, w) if (x != w) { zjc::terminate(0x04, y, z); }

#define __HASHIT__(x) #x
#define HASHIT(x) __HASHIT__(x)

#define __LINESTRING__ HASHIT(__LINE__)

#define __ERRLOC__ zjc::evil_shpath(__FILE__), " on func ", __func__, " line ", __LINESTRING__
#define GETLOC zjc::evil_geterrloc(__ERRLOC__)

//  - --- - --- - --- - --- -

#define retchar(x) reinterpret_cast<char*>(x)
#define retconcha(x) reinterpret_cast<concha*>(x)

#endif // ZAJEC_EVIL_H
