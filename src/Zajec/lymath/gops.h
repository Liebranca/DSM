#ifndef ZAJEC_GOPS_H
#define ZAJEC_GOPS_H

//  - --- - --- - --- - --- -

// Generic operations using templates.
namespace gops {

    // Get max value in (v1, v2)
    template<typename T>
    T max(T v1, T v2)               { if (v1 > v2) { return v1; } return v2;                                }

    // Get min value in (v1, v2)
    template<typename T>
    T min(T v1, T v2)               { if (v1 < v2) { return v1; } return v2;                                }

    // Substract min from max in (v1, v2) and get result
    template<typename T>
    T submax(T v1, T v2)            { return max(v1, v2) - min(v1, v2);                                     }

    // Substract max from min in (v1, v2) and get result
    template<typename T>
    T submin(T v1, T v2)            { return min(v1, v2) - max(v1, v2);                                     }

//  - --- - --- - --- - --- -
    
    // Get |v| or absolute value of v.
    template<typename T>
    T abs(T v)                      { if( v<0 ) { v *= -1; } return v;                                      }

    // Compute v^e, or v to the eth power.
    template<typename T>
    T pow(T v, int e)               { if (e < 0) { return root((float)v, abs(1/e)); }
                                      else if(e == 0) { return 1; }
                                      while(e > 0) { v *= e; e--; } return v;                               }

    // Babylonian approximation of square root
    template<typename T>
    float root(T v, int e)          { float r = v; while(r - (v/r) > 0.001f)
                                    { r = (r + (v/r)) / 2; } return r;                                      }

//  - --- - --- - --- - --- -
    
    // Get if v1 is at least m close to v2
    template<typename T>
    bool approa(T v1, T v2, T m)    { return v2 - m < v1 && v1 < v2 + m;                                    }
    
    // Clamp v to range (min, max)
    template<typename T>
    T clamp(T v, T min, T max)      { if (v < min) { v = min; } if (v > max) { v = max; } return v;         }
    
    // Cheaper linear interpolation
    template<typename T>
    T ilerp(T i1, T i2, T l, T p)   { float d = (float)fabs(i1 - i2) / l;
                                      int s = 1; if (i1 > i2) { s = -1; }
                                      return i1 + d * (p * s);                                              }

//  - --- - --- - --- - --- -

    // Clamp |v| to range (min, max)
    template<typename T>
    T absclamp(T v, T min, T max)   { int dirn = 1; if (v < 0) { dirn = -1; }
                                      if (abs(v) < min) { v = min; } if (abs(v) > max) { v = max; }
                                      return v * dirn;                                                      }
    
    // Ensure v1 is on the same polarity as v2
    template<typename T>
    T unipolar(T v1, T v2)          { if ((v1 < 0 && v2 > 0) || (v1 > 0 && v2 < 0))
                                      { return -v1; } return v1;                                            }
    
    // This is 100% ICWH... isn't it, Lyeb?
    template<typename T>
    float codist(T x1, T x2)        { return root(pow(x2 - x1, 2), 2);                                      }

};

//  - --- - --- - --- - --- -

namespace lymath {
    using namespace gops;
}

#endif // ZAJEC_GOPS_H
