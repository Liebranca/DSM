#include "GAOL_Sphere.h"

#include "ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

int GAOL_COLLISION_SPHERE::boxIsect
                           (COLFACE* planes) {

    int isInside = 0;

    for(uint i = 0; i < 6; i++)
    {
        if( planes[i].pointTest(origin) < 0 )  { if(dot(planes[i].centroid - origin, planes[i].normal) < radius)
                                               { return -1; }                                                           }

        else                                   { isInside++;                                                            }
    }

    return isInside == 6;                                                                                               }

//  - --- - --- - --- - --- -

