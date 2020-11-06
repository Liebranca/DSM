#include "DA_Camera.h"

#include "lymath/ZJC_VOPS.h"

#include "DA_WorldManager.h"
#include "DA_Occluder.h"

#include "lyutils/ZJC_Evil.h"
#include "lymath/ZJC_GOPS.h"
#include "spatial/ZJC_Transform.h"

#include "../Chasm/wind/chMANG.h"

#include <stdio.h>

DA_CAMERA*       actcam          = NULL;
glm::mat4        actcam_viewproj = IDENTITY;
glm::vec3        actcam_fwd      = { 0,0,0 };
glm::vec3        actcam_pos      = { 0,0,0 };

//  - --- - --- - --- - --- -

DA_CAMERA::DA_CAMERA(const glm::vec3& pos,
                     float fov,
                     float width,
                     float height,
                     float zNear,
                     float zFar)                {

    this->pos     = pos;

    float fpos[3] = { pos.x, pos.y, pos.z };

    fwd           = glm::vec3(0, 0, 1);
    up            = glm::vec3(0, 1, 0);

    frustum = new viewFrustum(glm::vec2(width, height), fov, 0, SIN_ORTHOSCALE, zNear, zFar);

    DA_grid_findpos   (curcell.worldpos, fpos            );
    DA_grid_findabspos(curcell.gridpos,  curcell.worldpos);                                                             }

DA_CAMERA::~DA_CAMERA()                         { WARD_EVIL_MFREE(nearcells); delete frustum;                           }

//  - --- - --- - --- - --- -

void DA_CAMERA::projOrtho(float width,
                          float height)         {

    frustum->isOrtho = 1;
    frustum->projection = getOrthographicProjection(width, height, SIN_ORTHOSCALE, frustum->zNear, frustum->zFar);      }

void DA_CAMERA::projPersp(float width,
                          float height)         {

    frustum->isOrtho = 0;
    frustum->projection = getPerspectiveProjection (width, height, frustum->FOV,   frustum->zNear, frustum->zFar);      }

void DA_CAMERA::updateFrustum()                 { frustum->buildPlanes(pos, fwd, up);                                   }

//  - --- - --- - --- - --- -

void DA_CAMERA::onAreaChange()                  {

    WARD_EVIL_MFREE(nearcells);
    nearcells = (DAGCI*) evil_malloc(DA_grid_getFrustumFac(), sizeof(DAGCI));                                           }

void DA_CAMERA::cellCulling
        (uint         num_cells,
         viewFrustum* lightFrustums[SIN_MAX_LAMPS],
         uint         num_dirlights)            {

    cint h                    = DA_CELL_HALF;
    cint w                    = DA_CELL_HALF;

    for(uint i = 0; i < num_cells; i++)
    {

        int ox = (nearcells[i].worldpos[0] * DA_CELL_SIZE);
        int oy = (nearcells[i].worldpos[1] * DA_CELL_SIZE);
        int oz = (nearcells[i].worldpos[2] * DA_CELL_SIZE);
        glm::vec3 origin(ox, oy, oz);

        glm::vec3 cellbounds[8] = { origin + glm::vec3(  w, -h, -w), origin + glm::vec3(  w,  h, -w),
                                    origin + glm::vec3(  w,  h,  w), origin + glm::vec3(  w, -h,  w),
                                    origin + glm::vec3( -w, -h, -w), origin + glm::vec3( -w,  h, -w),
                                    origin + glm::vec3( -w,  h,  w), origin + glm::vec3( -w, -h,  w) };

        COLSPHERE cellsphere (origin, w );
        COLBOX    cellbox    (cellbounds);

        DA_grid_setInFrustum(nearcells[i].gridpos, frustum->cellInside(&cellsphere, &cellbox));

        for(uint j = 0; j < num_dirlights; j++)
        { DA_grid_setInLightFrustum(nearcells[i].gridpos, j, lightFrustums[j]->cellInside(&cellsphere, &cellbox)); }

    }

    prevframe_cells = num_cells;                                                                                        }

void DA_CAMERA::resetCulling()                  { for(uint i = 0; i < prevframe_cells; i++)
                                                { DA_grid_setInFrustum(nearcells[i].gridpos, 0); }                      }

bool DA_CAMERA::cageInFrustum(COLBOX* box)      { return frustum->boxIsect(box);                                        }
int  DA_CAMERA::sphInFrustum (COLSPHERE* sph)   { return frustum->sphereIsect(sph);                                     }

bool DA_CAMERA::pointsInFrustum
    (glm::vec3* points, uint num_points)        { return frustum->pointsInside(points, num_points);                     }

bool DA_CAMERA::pointInFrustum(
                    glm::vec3& point)           { return frustum->pointInside(point);                                   }

int DA_CAMERA::frustumIsect(viewFrustum* other) { return frustum->frustumIsect(other);                                  }

//  - --- - --- - --- - --- -

void DA_CAMERA::move(glm::vec3 mvec,
                     bool local)                {

    update             = true;
    glm::vec3 displace = { 0,0,0 };

    if (local)
    {
        float xfac = fabs(up.x); if (fwd.x < 0) { xfac *= -1;                                                           }
        float zfac = fabs(up.z); if (fwd.z < 0) { zfac *= -1;                                                           }

        glm::vec3 noup =                        { fwd.x + xfac, 0, fwd.z + zfac                                         };

        if (mvec[2])                            { displace += noup * mvec[2];                                           }
        if (mvec[0])                            { displace -= glm::cross(noup, yAxis) * mvec[0];                        }

        displace.y = mvec.y;
    }

    else                                        { displace = mvec;                                                      }

    pos += displace;

    float fpos[3] =   { pos.x, pos.y, pos.z              };
    DA_grid_findpos   (curcell.worldpos, fpos            );
    DA_grid_findabspos(curcell.gridpos,  curcell.worldpos);                                                             }

void DA_CAMERA::rotate(glm::vec3 rvec)          {

    update = true;
    pitch += rvec.y; yaw += rvec.x;

    if      (pitch < -1.49f)                    { pitch = -1.49f;                                                       }
    else if (pitch >  1.49f)                    { pitch =  1.49f;                                                       }

    glm::vec3 hAxis = glm::normalize(glm::cross(yAxis, fwd));

    fwd.x           = sin(yaw  ) * cos(pitch);
    fwd.z           = cos(yaw  ) * cos(pitch);
    fwd.y           = sin(pitch);

    fwd             = glm::normalize(fwd);
    up              = glm::normalize(glm::cross(fwd, hAxis));                                                           }

//  - --- - --- - --- - --- -

void   camlook()                                { fpair motion = getMouseMotion();
                                                  actcam->rotate({-motion.x, -motion.y, 0.0f}); onMouseMotion();        }

