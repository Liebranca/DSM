#ifndef ZAJEC_DYNARRAY_H
#define ZAJEC_DYNARRAY_H

#include "../lyutils/ZJC_Evil.h"
#include "../ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

namespace zjc {

template<typename T>
struct dynarray                             {

    uint        size;
    T*          buff;    
    
    dynarray    (uint s)
                :size(s), buff(0)           { buff = (T*) evil_malloc(size, sizeof(T));                    }

    ~dynarray   ()                          { evil_free(&buff);                                                 }
    dynarray    ()                          {                                                                   }
    
    T& operator [] (uint i)                 { return *(buff + i);                                               }
    
                                                                                                                };

typedef dynarray<float> fArray;
                                                                                                                }

//  - --- - --- - --- - --- -

#endif // ZAJEC_DYNARRAY_H
