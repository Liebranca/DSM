#include "GAOL_Box.h"
#include "GAOL_Constants.h"

#include "lymath/ZJC_GOPS.h"

//  - --- - --- - --- - --- -

GAOL_COLLISION_BOX::GAOL_COLLISION_BOX
                        (glm::vec3 co[8])       {

    faces[0] = COLFACE(co[0], co[4], co[7]);
    faces[1] = COLFACE(co[2], co[1], co[5]);
    faces[2] = COLFACE(co[0], co[4], co[5]);
    faces[3] = COLFACE(co[2], co[6], co[7]);
    faces[4] = COLFACE(co[5], co[6], co[7]);
    faces[5] = COLFACE(co[1], co[2], co[3]);

    faces[0].normal = -faces[0].normal;
    faces[0].d      = -faces[0].d;

    faces[4].normal = -faces[4].normal;
    faces[4].d      = -faces[4].d;

    area[0] = maxf(faces[0].edges[0].length,
                   faces[0].edges[1].length);

    area[1] = faces[2].edges[1].length;

    points[0] = co[0]; points[1] = co[1];
    points[2] = co[2]; points[3] = co[3];
    points[4] = co[4]; points[5] = co[5];
    points[6] = co[6]; points[7] = co[7];                                                                               }

//  - --- - --- - --- - --- -

GAOL_COLLISION_BOX
 GAOL_COLLISION_BOX::selfProjection
                        (glm::vec3 dirn)        { 

    glm::vec3 projected_points[8];
    for (uint i = 0; i < 8; i++)                { projected_points[i] = (this->points[i] + dirn);                       }

    return COLBOX(projected_points);                                                                                    }

//  - --- - --- - --- - --- -

RAYDATA* GAOL_COLLISION_BOX::genCage
                        (COLFACE* other)        {

    COLLINE l1, l2, l3, l4;

    l1 = COLLINE(faces[0].edges[1].p2, faces[0].p4);
    l2 = COLLINE(faces[0].p4, faces[0].edges[0].p1);
    l3 = COLLINE(faces[1].edges[1].p2, faces[1].p4);
    l4 = COLLINE(faces[1].p4, faces[1].edges[0].p1);

    RAYDATA cage[12] =
    {

        other->rayIsectPoint(&faces[0].edges[0]),
        other->rayIsectPoint(&faces[0].edges[1]),
        other->rayIsectPoint(&l1),
        other->rayIsectPoint(&l2),
             
        other->rayIsectPoint(&faces[1].edges[0]),
        other->rayIsectPoint(&faces[1].edges[1]),
        other->rayIsectPoint(&l3),
        other->rayIsectPoint(&l4),
             
        other->rayIsectPoint(&faces[2].edges[1]),
        other->rayIsectPoint(&faces[3].edges[1]),
        other->rayIsectPoint(&faces[4].edges[1]),
        other->rayIsectPoint(&faces[5].edges[1])
    };

    return cage;                                                                                                        }

//  - --- - --- - --- - --- -

bool GAOL_COLLISION_BOX::bottomFaceCol
                (COLFACE* other, float fac)     {

    glm::vec3 vel(0, fac, 0);
    COLLINE l1;

    l1 = COLLINE(faces[0].edges[0].p1,
                 faces[0].edges[0].p1 - vel);

    if (other->rayIsectPoint(&l1).hitObject)    { return true;                                                          }

    l1 = COLLINE(faces[0].edges[0].p2,
                 faces[0].edges[0].p2 - vel);

    if (other->rayIsectPoint(&l1).hitObject)    { return true;                                                          }

    l1 = COLLINE(faces[0].edges[1].p2,
                 faces[0].edges[1].p2 - vel);

    if (other->rayIsectPoint(&l1).hitObject)    { return true;                                                          }

    l1 = COLLINE(faces[0].p4,
                 faces[0].p4 - vel);

    if (other->rayIsectPoint(&l1).hitObject)    { return true;                                                          }

    return false;                                                                                                       }

//  - --- - --- - --- - --- -

RAYDATA GAOL_COLLISION_BOX::raytest
               (GAOL_COLLISION_BOX*  other,
                COLLINE*             ray)       {

    RAYDATA rd, test;

    for (int i = 2; i < 6; i++)
    {
        test = other->faces[i].rayIsectPoint(ray);
        if (test.hitObject)                     { rd = test; break;                                                     }
    }

    if (rd.hitObject)
    {
        if (glm::distance(rd.point, ray->centroid)
           > (ray->length * 0.5))               { rd.hitObject = false;                                                 }
    }

    return rd;                                                                                                          }

//  - --- - --- - --- - --- -

bool GAOL_COLLISION_BOX::inDomainOf
                (GAOL_COLLISION_BOX* b)         {

    glm::vec3& d0 = b->faces[0].edges[0].p1;
    glm::vec3& d1 = b->faces[0].edges[1].p2;
    float bdxm = minf(d0.x, d1.x), bdxp = maxf(d0.x, d1.x);
    float bdzm = minf(d0.z, d1.z), bdzp = maxf(d0.z, d1.z);

    d0 = this->faces[0].edges[0].p1;
    d1 = this->faces[0].edges[1].p2;
    float tdxm = minf(d0.x, d1.x), tdxp = maxf(d0.x, d1.x);
    float tdzm = minf(d0.z, d1.z), tdzp = maxf(d0.z, d1.z);

    bool in_xdom = (bdxm <= tdxm && tdxp <= bdxp) || (bdxm <= tdxp && tdxm <= bdxp);
    bool in_zdom = (bdzm <= tdzm && tdzp <= bdzp) || (bdzm <= tdzp && tdzm <= bdzp);

    return in_xdom && in_zdom;                                                                                          }

//  - --- - --- - --- - --- -

Booflo GAOL_COLLISION_BOX::groundCheck
                    (GAOL_COLLISION_BOX* other,
                     float               fac)   {

    if (!inDomainOf(other))                     { return { false, 0.0f };                                               }

    Booflo gchk = { false, {0.0f} };
    for (int i = 0; i < 6; i++)
    {
        if (dot(this->faces[0].normal,
               -other->faces[i].normal)

           > GAOL_COLNORMAL           )         { if (this->bottomFaceCol(&other->faces[i], fac))
                                                { gchk = { true, other->faces[i].centroid.y }; break; }                 }
    }

    return gchk;                                                                                                        }

//  - --- - --- - --- - --- -

bool GAOL_COLLISION_BOX::pointInside
                            (glm::vec3 point)   {

    uint count = 0;
    for (int i = 0; i < 6; i++)
    {
        if ( (dot(faces[i].normal, point)
             + faces[i].d)

             <= GAOL_COLMARGIN          )       { count++;                                                              }
    }

    return count == 6;                                                                                                  }

//  - --- - --- - --- - --- -

bool GAOL_COLLISION_BOX::cageIsect
                    (RAYDATA cage[12])          {

    for (uint i = 0; i < 12; i++)               { if(cage[i].hitObject)
                                                { if(pointInside(cage[i].point)) { return true; } }                     }

    return false;                                                                                                       }

bool GAOL_COLLISION_BOX::rayToBox
                    (RAYDATA ray)               {

    if(ray.hitObject)                           { if(pointInside(ray.point)) { return true; }                           }
    return false;                                                                                                       }

bool GAOL_COLLISION_BOX::cageIsect_opt
                    (COLFACE* other)            {

    if     (rayToBox(other->rayIsectPoint(&faces[2].edges[1]))) { return true; }
    else if(rayToBox(other->rayIsectPoint(&faces[3].edges[1]))) { return true; }
    else if(rayToBox(other->rayIsectPoint(&faces[4].edges[1]))) { return true; }
    else if(rayToBox(other->rayIsectPoint(&faces[5].edges[1]))) { return true; }

    else if(rayToBox(other->rayIsectPoint(&faces[0].edges[0]))) { return true; }
    else if(rayToBox(other->rayIsectPoint(&faces[0].edges[1]))) { return true; }

    else if(rayToBox(other->rayIsectPoint(&faces[1].edges[0]))) { return true; }
    else if(rayToBox(other->rayIsectPoint(&faces[1].edges[1]))) { return true; }

    else if(rayToBox(other->rayIsectPoint(&COLLINE(faces[0].edges[1].p2, faces[0].p4)))) { return true; }
    else if(rayToBox(other->rayIsectPoint(&COLLINE(faces[0].p4, faces[0].edges[0].p1)))) { return true; }
    else if(rayToBox(other->rayIsectPoint(&COLLINE(faces[1].edges[1].p2, faces[1].p4)))) { return true; }
    else if(rayToBox(other->rayIsectPoint(&COLLINE(faces[1].p4, faces[1].edges[0].p1)))) { return true; }

    return false;                                                                                                       }

bool GAOL_COLLISION_BOX::lineInside
                        (COLLINE* ray)          {

    glm::vec3 p;
    for (int i = 0; i < int(ray->length); i++)
    {
        p = ray->pointAlong(i * 0.1f);
        if (this->pointInside(p))               { return true;                                                          }
    }

    return false;                                                                                                       }

//  - --- - --- - --- - --- -

bool GAOL_COLLISION_BOX::pointInDomain
                            (glm::vec3 v)       {

    glm::vec3& d0 = this->faces[0].edges[0].p1;
    glm::vec3& d1 = this->faces[0].edges[1].p2;

    float bdxm = minf(d0.x, d1.x) - GAOL_COLMARGIN;
    float bdxp = maxf(d0.x, d1.x) + GAOL_COLMARGIN;
    float bdym = minf(d0.y, d1.y) - GAOL_COLMARGIN;
    float bdyp = maxf(d0.y, d1.y) + GAOL_COLMARGIN;
    float bdzm = minf(d0.z, d1.z) - GAOL_COLMARGIN;
    float bdzp = maxf(d0.z, d1.z) + GAOL_COLMARGIN;

    bool in_xdom = (bdxm <= v.x && v.x <= bdxp);
    bool in_ydom = (bdym <= v.y && v.y <= bdyp);
    bool in_zdom = (bdzm <= v.z && v.z <= bdzp);

    return in_xdom && in_zdom;                                                                                          }

//  - --- - --- - --- - --- -

glm::vec3 GAOL_COLLISION_BOX::boxToBox
                    (GAOL_COLLISION_BOX* b,
                     glm::vec3 dirn)            {

    if (!inDomainOf(b))                         { return { 0,0,0 };                                                     }

    bool col = false;
    glm::vec3 n = { 0,0,0 };

    for (uint i = 0; i < 6; i++)
    {
        if (dot(dirn, -b->faces[i].normal)

            > GAOL_COLNORMAL             )      { col = cageIsect(genCage(&b->faces[i]));
                                                  if (col) { n += b->faces[i].normal; break; }                          }
    }

    return n;                                                                                                           }

//  - --- - --- - --- - --- -

