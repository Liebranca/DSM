#ifndef ZAJEC_DYNARRAY_H
#define ZAJEC_DYNARRAY_H

#include "../lyutils/evil.h"
#include "../unsigdefs.h"
#include "../constdefs.h"

//  - --- - --- - --- - --- -

namespace zjc {

template<typename T>
struct dynarray                             {

    uint        size;
    T*          buff;    
    
    dynarray    (uint s,
                 concha* loc)
                :size(s), buff(0)           { buff = (T*) evil_malloc(size, sizeof(T), loc);                    }

    ~dynarray   ()                          { evil_free(&buff);                                                 }
    dynarray    ()                          {                                                                   }
    
    T& operator [] (uint i)                 { return *(buff + i);                                               }
    
                                                                                                                };

typedef dynarray<float> fArray;
                                                                                                                }

//  - --- - --- - --- - --- -

#endif // ZAJEC_DYNARRAY_H
