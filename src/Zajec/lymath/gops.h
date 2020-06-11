#ifndef ZAJEC_GOPS_H
#define ZAJEC_GOPS_H

#include <iostream>
#include "../lyarr/dynarray.h"

//  - --- - --- - --- - --- -

namespace zjc                       {

    template<typename T>
    T max(T v1, T v2)               { if (v1 > v2) { return v1; } return v2;                                }
    template<typename T>
    T min(T v1, T v2)               { if (v1 < v2) { return v1; } return v2;                                }
    template<typename T>
    T submin(T v1, T v2)            { return max(v1, v2) - min(v1, v2);                                     }
    template<typename T>
    T submax(T v1, T v2)            { return min(v1, v2) - max(v1, v2);                                     }

//  - --- - --- - --- - --- -
    
    template<typename T>
    T abs(T v)                      { if( v<0 ) { v *= -1; } return v;                                      }
    template<typename T>
    T pow(T v, int e)               { if (e < 0) { return root((float)v, abs(e)); }
                                      else if(e == 0) { return 1; } T result = v;
                                      while(e > 0) { result *= v; e--; } return result;                     }
    template<typename T>
    float root(T v, int e)          { float r = v; float d = 1/e; while(r - (v/r) > 0.001f)
                                    { r = (r + (v/r)) * d; } return r;                                      }

//  - --- - --- - --- - --- -
    
    template<typename T>
    bool approa(T v1, T v2, T m)    { return v2 - m < v1 && v1 < v2 + m;                                    }

    template<typename T>
    T approad(T v1, T v2, T m)      { if(v2 - m < v1 && v1 < v2 + m) { return submin(v1, v2); }
                                      return 999;                                                           }
    template<typename T>
    T clamp(T v, T min, T max)      { if (v < min) { v = min; } if (v > max) { v = max; } return v;         }
    template<typename T>
    T ilerp(T i1, T i2, T l, T p)   { float d = (float)fabs(i1 - i2) / l;
                                      int s = 1; if (i1 > i2) { s = -1; }
                                      return i1 + d * (p * s);                                              }

//  - --- - --- - --- - --- -

    template<typename T>
    T absclamp(T v, T min, T max)   { int dirn = 1; if (v < 0) { dirn = -1; }
                                      if (abs(v) < min) { v = min; } if (abs(v) > max) { v = max; }
                                      return v * dirn;                                                      }
    template<typename T>
    T unipolar(T v1, T v2)          { if ((v1 < 0 && v2 > 0) || (v1 > 0 && v2 < 0))
                                    { return -v1; } return v1;                                              }
    template<typename T>
    float codist(T x1, T x2)        { return root(pow(x2 - x1, 2), 2);                                      }

//  - --- - --- - --- - --- -
    template<typename T, typename N>
    class vRange {

    private:
        size_t mag; N step;
        dynarray<T> values;
    
    public:
        vRange(size_t _mag,
               N      _step,
               cchar* loc)
             : step(_step),
               mag(_mag),
               values(mag, loc)
                                    { for(unsigned int i = 0; i < mag; i++)
                                    { values[i] = (i*step); }                                               }

        ~vRange()                   { zjc::evil_free(&values.buff);                                         }
   
        T& operator [] (size_t i)   { return values[i];                                                     }
        size_t size()               { return values.size;                                                   }

        size_t take_closest (T v)   { N s = step/2; T dist = 999; T newdist; size_t closest = 0;
                                    for (unsigned int i = 0; i < mag; i++)
                                    { newdist = approad(v, values[i], s); if ( newdist < dist )
                                    { dist = newdist; closest = i; } } return closest;                      }

                                                                                                            };

    typedef vRange<float, float> fvRange;
                                                                                                            }
//  - --- - --- - --- - --- -


#endif // ZAJEC_GOPS_H
