#include "DA_Camera.h"

#include "ZJC_CommonTypes.h"
#include "spatial/ZJC_Transform.h"

#include <stdio.h>

DA_CAMERA* active_camera   = NULL;
glm::mat4  actcam_viewproj = IDENTITY;
glm::vec3  actcam_fwd      = { 0,0,0 };
glm::vec3  actcam_pos      = { 0,0,0 };

//  - --- - --- - --- - --- -

DA_CAMERA::DA_CAMERA(const glm::vec3& pos,
                     float fov,
                     float aspect,
                     float zNear,
                     float zFar)                {

    perspective = glm::perspective(glm::radians(fov), aspect, zNear, zFar);

    this->pos   = pos;
    this->zNear = zNear;
    this->zFar  = zFar;

    this->hNear = 2 * tan(fov / 2) * zFar;
    this->wNear = this->hNear * aspect;
    this->hFar  = 2 * tan(fov / 2) * zFar;
    this->wFar  = hFar * aspect;

    fwd         = glm::vec3(0, 0, 1);
    up          = glm::vec3(0, 1, 0);                                                                                   }

//  - --- - --- - --- - --- -

void DA_CAMERA::getFrustum()                    {

    glm::vec3 hAxis = glm::normalize(glm::cross(fwd, up));

    glm::vec3 fc    = pos + fwd * zFar;
    glm::vec3 ftl   = fc + (up * (hFar / 2)) - (hAxis * (wFar / 2));
    glm::vec3 ftr   = fc + (up * (hFar / 2)) + (hAxis * (wFar / 2));
    glm::vec3 fbl   = fc - (up * (hFar / 2)) - (hAxis * (wFar / 2));
    glm::vec3 fbr   = fc - (up * (hFar / 2)) + (hAxis * (wFar / 2));

    glm::vec3 nc    = pos + fwd * zNear;
    glm::vec3 ntl   = nc + (up * (hNear / 2)) - (hAxis * (wNear / 2));
    glm::vec3 ntr   = nc + (up * (hNear / 2)) + (hAxis * (wNear / 2));
    glm::vec3 nbl   = nc - (up * (hNear / 2)) - (hAxis * (wNear / 2));
    glm::vec3 nbr   = nc - (up * (hNear / 2)) + (hAxis * (wNear / 2));

    planes[0]       = COLFACE(ntr,ntl,ftl);
    planes[1]       = COLFACE(nbl,nbr,fbr);
    planes[2]       = COLFACE(ntl,nbl,fbl);
    planes[3]       = COLFACE(nbr,ntr,fbr);
    planes[4]       = COLFACE(ntl,ntr,nbr);
    planes[5]       = COLFACE(ftr,ftl,fbl);                                                                             }

//  - --- - --- - --- - --- -

bool DA_CAMERA::rectInFrustum(
                    glm::vec3* bounds)          {

    for (uint i = 0; i < 6; i++)                { if (pointInFrustum(bounds[i]))
                                                { return true; }                                                        }

    return false;                                                                                                       }

bool DA_CAMERA::pointInFrustum(
                    glm::vec3& point)           {

    for (uint i = 0; i < 6; i++)                { if ( (dot(planes[i].normal, point)+planes[i].d) < 0)
                                                { return false; }                                                       }

    return true;                                                                                                        }

//  - --- - --- - --- - --- -

void DA_CAMERA::move(glm::vec3& mvec,
                     bool local)                {

    update = true;

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

    pos += displace;                                                                                                    }

void DA_CAMERA::rotate(glm::vec3& rvec)         {

    update = true;

    pitch += rvec.y; yaw += rvec.x;

    if      (pitch < -1.49f)                    { pitch = -1.49f;                                                       }
    else if (pitch >  1.49f)                    { pitch = 1.49f;                                                        }

    glm::vec3 hAxis = glm::normalize(glm::cross(yAxis, fwd));

    fwd.x           = sin(yaw) * cos(pitch);
    fwd.y           = sin(pitch);
    fwd.z           = cos(yaw) * cos(pitch);

    fwd             = glm::normalize(fwd);
    up              = glm::normalize(glm::cross(fwd, hAxis));                                                           }

