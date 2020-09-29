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

    FOV           = fov;
    float aspect  = width / height;
    projection    = glm::perspective(glm::radians(FOV), aspect, zNear, zFar);

    this->pos     = pos;
    this->zNear   = zNear;
    this->zFar    = zFar;

    float fpos[3] = { pos.x, pos.y, pos.z };

    this->hNear   = 2 * tan(fov / 2) * zNear;
    this->wNear   = this->hNear * aspect;
    this->hFar    = 2 * tan(fov / 2) * zFar;
    this->wFar    = hFar * aspect;
                  
    fwd           = glm::vec3(0, 0, 1);
    up            = glm::vec3(0, 1, 0);

    DA_grid_findpos   (curcell.worldpos, fpos            );
    DA_grid_findabspos(curcell.gridpos,  curcell.worldpos);                                                             }

DA_CAMERA::~DA_CAMERA()                         { WARD_EVIL_MFREE(nearcells);                                           }

//  - --- - --- - --- - --- -

void DA_CAMERA::projOrtho(float width,
                          float height)         { projection = getOrthographicProjection
                                                                    (width, height, orthoScale, zNear, zFar);           }

void DA_CAMERA::projPersp(float width,
                          float height)         { projection = getPerspectiveProjection
                                                                                (width, height, FOV, zNear, zFar);      }

void DA_CAMERA::getFrustum()                    {

    glm::vec3 hAxis = glm::normalize(glm::cross(fwd, up));

    glm::vec3 fc    = pos + fwd * zFar;
    glm::vec3 ftl   = fc + (up * (hFar / 2)) + (hAxis * (wFar / 2));
    glm::vec3 ftr   = fc + (up * (hFar / 2)) - (hAxis * (wFar / 2));
    glm::vec3 fbl   = fc - (up * (hFar / 2)) + (hAxis * (wFar / 2));
    glm::vec3 fbr   = fc - (up * (hFar / 2)) - (hAxis * (wFar / 2));

    glm::vec3 nc    = pos + fwd * zNear;
    glm::vec3 ntl   = nc + (up * (hNear / 2)) + (hAxis * (wNear / 2));
    glm::vec3 ntr   = nc + (up * (hNear / 2)) - (hAxis * (wNear / 2));
    glm::vec3 nbl   = nc - (up * (hNear / 2)) + (hAxis * (wNear / 2));
    glm::vec3 nbr   = nc - (up * (hNear / 2)) - (hAxis * (wNear / 2));

    planes[0]       = COLFACE(nbl,nbr,fbr); // bottom
    planes[1]       = COLFACE(ftl,ntl,ntr); // top
    planes[2]       = COLFACE(nbl,nbr,ntr); // front
    planes[3]       = COLFACE(ftl,ftr,fbr); // back
    planes[4]       = COLFACE(ntr,ftr,fbr); // right
    planes[5]       = COLFACE(ntl,ftl,fbl); // left

    planes[0].normal = -planes[0].normal;
    planes[0].d      = -planes[0].d;

    planes[4].normal = -planes[4].normal;
    planes[4].d      = -planes[4].d;                                                                                    }

//  - --- - --- - --- - --- -

void DA_CAMERA::onAreaChange()                  {

    WARD_EVIL_MFREE(nearcells);
    nearcells = (DAGCI*) evil_malloc(DA_grid_getFrustumFac(), sizeof(DAGCI));                                           }

void DA_CAMERA::cellCulling(uint num_cells)     {

    cint h                    = DA_CELL_HALF;
    cint w                    = DA_CELL_HALF;

    for(uint i = 0; i < num_cells; i++)
    {

        int ox = (nearcells[i].worldpos[0] * DA_CELL_SIZE);
        int oy = (nearcells[i].worldpos[1] * DA_CELL_SIZE);
        int oz = (nearcells[i].worldpos[2] * DA_CELL_SIZE);
        glm::vec3 origin(ox, oy, oz);
        COLSPHERE boundsphere(origin, DA_CELL_HALF);

        int cellInFrustum = boundsphere.boxIsect(planes);
        int sphIsect = 0;

        if(cellInFrustum < 0)
        {
            glm::vec3 cellbounds[8] = { origin + glm::vec3(  w, -h, -w), origin + glm::vec3(  w,  h, -w),
                                        origin + glm::vec3(  w,  h,  w), origin + glm::vec3(  w, -h,  w),
                                        origin + glm::vec3( -w, -h, -w), origin + glm::vec3( -w,  h, -w),
                                        origin + glm::vec3( -w,  h,  w), origin + glm::vec3( -w, -h,  w) };

            COLBOX cellbox(cellbounds); cellInFrustum = (int) cageInFrustum(&cellbox);
        }

        DA_grid_setInFrustum(nearcells[i].gridpos, cellInFrustum);
    }

    prevframe_cells = num_cells;                                                                                        }

void DA_CAMERA::resetCulling()                  { for(uint i = 0; i < prevframe_cells; i++)
                                                { DA_grid_setInFrustum(nearcells[i].gridpos, 0); }                      }

bool DA_CAMERA::cageInFrustum(COLBOX* box)      {

    for(uchar j = 0; j < 6; j++)                { if(box->cageIsect_opt(planes+j)) { return true; }                     }

    return false;                                                                                                       }

int  DA_CAMERA::sphInFrustum (COLSPHERE* sph)   { return sph->boxIsect(planes);                                         }

bool DA_CAMERA::rectInFrustum(
                    glm::vec3 bounds[8])        {

    for (uint i = 0; i < 8; i++)                { if (this->pointInFrustum(bounds[i]))
                                                { return true; }                                                        }

    return false;                                                                                                       }

bool DA_CAMERA::pointInFrustum(
                    glm::vec3 point)            {

    for (uint i = 0; i < 6; i++)                { if ( planes[i].pointTest(point) < 0)
                                                { return false; }                                                       }

    return true;                                                                                                        }

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

