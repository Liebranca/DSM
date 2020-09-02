#ifndef __GAOL_BOUNDS_H__
#define __GAOL_BOUNDS_H__

#include "GAOL_Box.h"
#include "GAOL_Sphere.h"
#include "ZJC_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

typedef struct GAOL_COLLISION_BOUNDS            {

    glm::vec4  points[8];
    COLBOX*    box;
    COLSPHERE* sphere;

    GAOL_COLLISION_BOUNDS()                     { box = new COLBOX(); sphere = new COLSPHERE();                         }
    ~GAOL_COLLISION_BOUNDS()                    { if(box) { delete box; } if(sphere) { delete sphere; }                 }

    GAOL_COLLISION_BOUNDS(glm::vec4 _points[8]);

    void genBox(glm::mat4& model);

} COLBOUNDS;

//  - --- - --- - --- - --- -

void       GAOL_boundbucket_init();
void       GAOL_boundbucket_end ();

void       GAOL_boundbucket_add (glm::vec4 points[8], ushort loc);
COLBOUNDS* GAOL_boundbucket_get (ushort loc);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif //__GAOL_BOUNDS_H__

