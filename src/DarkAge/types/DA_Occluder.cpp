#include "DA_Occluder.h"

#include "ZJC_CommonTypes.h"
#include "lymath/ZJC_GOPS.h"
#include "lymath/ZJC_VOPS.h"

//  - --- - --- - --- - --- -

void DA_OCCLUDER::setBox    (COLBOX* _box)      { box       = _box;

                                                  bounds[0] = box->origin; bounds[1] = box->origin;
                                                  bounds[2] = box->origin; bounds[3] = box->origin;
                                                  bounds[4] = box->origin; bounds[5] = box->origin;
                                                  bounds[6] = box->origin; bounds[7] = box->origin;                     }

//  - --- - --- - --- - --- -

void DA_OCCLUDER::getFrustum(glm::vec3& eye,
                             glm::vec3& pos,
                             glm::vec3& up,
                             float zFar,
                             float hFar,
                             float wFar)        {

    glm::vec3 hAxis = glm::normalize(glm::cross(up, eye));

    glm::vec3 ntl(bounds[1].x, box->faces[1].p4.y, bounds[1].z);
    glm::vec3 ntr(bounds[5].x, box->faces[1].p4.y, bounds[5].z);
    glm::vec3 castdirn = {0,0,0}; uint num_dirns = 0;

    for(uint i = 2; i < 6; i++)
    { 
        if(dot(pos - box->faces[i].centroid, box->faces[i].normal) > 0)
        {
            COLFACE* plane = box->faces + i;
            castdirn += plane->normal; num_dirns++;

            glm::vec3 points[4] = { plane->edges[0].p1, plane->edges[0].p2,
                                    plane->edges[1].p2, plane->p4           };

            for(uint j = 0; j < 4; j++)
            {

                glm::vec3 p    = points[j];
                glm::vec3 ref  = { dot(p,   hAxis), dot(p,   up), dot(p,   eye) };
                glm::vec3 lolr = { dot(ntr, hAxis), dot(ntr, up), dot(ntr, eye) };
                glm::vec3 loll = { dot(ntl, hAxis), dot(ntl, up), dot(ntl, eye) };

                if     (ref.x < lolr.x) { ntr.x = p.x; ntr.z = p.z; }
                if     (ref.x > loll.x) { ntl.x = p.x; ntl.z = p.z; }

            }
        }
    }

    glm::vec3 nbl(ntl.x, box->faces[0].p4.y, ntl.z);
    glm::vec3 nbr(ntr.x, box->faces[0].p4.y, ntr.z);
    if(num_dirns) { castdirn /= num_dirns; }

    glm::vec3 dirn   = normalize( -castdirn + ((box->origin) - pos) );

    glm::vec3 ftl    = ntl + (dirn * zFar) + (up * (hFar/16)) + (hAxis * (wFar/16));
    glm::vec3 ftr    = ntr + (dirn * zFar) + (up * (hFar/16)) - (hAxis * (wFar/16));
    glm::vec3 fbl    = nbl + (dirn * zFar) - (up * (hFar/16)) + (hAxis * (wFar/16));
    glm::vec3 fbr    = nbr + (dirn * zFar) - (up * (hFar/16)) - (hAxis * (wFar/16));

    planes[0]        = COLFACE(nbl,nbr,fbr);
    planes[1]        = COLFACE(ntl,ntr,ftr);
    planes[2]        = COLFACE(nbl,nbr,ntr);
    planes[3]        = COLFACE(ftl,ntr,fbr);
    planes[4]        = COLFACE(ntr,ftr,fbr);
    planes[5]        = COLFACE(nbl,ntl,ftl);

    planes[0].normal = -planes[0].normal;
    planes[0].d      = -planes[0].d;

    planes[4].normal = -planes[4].normal;
    planes[4].d      = -planes[4].d;

    bounds[0]        = nbl;
    bounds[1]        = ntl;
    bounds[2]        = ftl;
    bounds[3]        = fbl;
                     
    bounds[4]        = nbr;
    bounds[5]        = ntr;
    bounds[6]        = ftr;
    bounds[7]        = fbr;                                                                                             }

//  - --- - --- - --- - --- -

int  DA_OCCLUDER::sphTest   (COLSPHERE* sph)    {

    for(uint i = 0; i < 6; i++)
    {

        if(planes[i].pointTest(sph->origin) > 0){ return 1;                                                             }

        else                                    { if(dot(sph->origin - planes[i].centroid,
                                                         -planes[i].normal) < sph->radius)

                                                { return -1; }                                                          }
    }

    return 0;                                                                                                           }

bool DA_OCCLUDER::getVisible(glm::vec3* bounds) {

    for(uint i = 0; i < 6; i++)                 { for(uint j = 0; j < 8; j++)
                                                { if(planes[i].pointTest(bounds[j]) > 0) { return true; }               }
                                                                                                                        }
    return false;                                                                                                       }

//  - --- - --- - --- - --- -

