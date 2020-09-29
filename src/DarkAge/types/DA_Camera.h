#ifndef __DARKAGE_CAMERA_H__
#define __DARKAGE_CAMERA_H__

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "GAOL_Face.h"
#include "GAOL_Sphere.h"
#include "GAOL_Box.h"

#include "../DA_CommonTypes.h"

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

class DA_CAMERA {

    public:

        DA_CAMERA(const glm::vec3& pos, float fov, float width, float height, float zNear, float zFar);

        virtual ~DA_CAMERA();

        glm::mat4 FAKEPROJ() const              { glm::vec3 fakepos(0,  1.8975f, 10);
return projection * glm::lookAt(fakepos, fakepos + glm::vec3(0, 0, -1), glm::vec3(0,1,0)); }

        glm::mat4 getViewProjection() const     { return projection * glm::lookAt(pos, pos + fwd, yAxis);               }
        inline glm::vec3 getEye()               { return glm::normalize(pos + fwd);                                     }

        void getFrustum();

        void onAreaChange  ();
        void cellCulling   (uint num_cells);
        void resetCulling  ();
        int  sphInFrustum  (COLSPHERE* sph);
        bool rectInFrustum (glm::vec3  bounds[8]);
        bool pointInFrustum(glm::vec3  point);
        bool cageInFrustum (COLBOX*    box);

        void projOrtho     (float width, float height);
        void projPersp     (float width, float height);

        void move(glm::vec3 mvec, bool local = false);
        void rotate(glm::vec3 rvec);
        
        float getPitch()                        { return pitch;                                                         }
        glm::vec3& getFwd()                     { return fwd;                                                           }
        glm::vec3& getPos()                     { return pos;                                                           }
        glm::vec3& getUp()                      { return up;                                                            }
        glm::vec3 getFwdCast(float dist = 3.5f) { return pos + (fwd * dist);                                            }
        void snapTo(glm::vec3 newpos)           { this->altpos = this->pos;
                                                  this->pos = newpos;                                                   }
        void    undoSnap()                      { this->pos = this->altpos;                                             }
        bool    getUpdate()                     { return this->update;                                                  }
        void    endUpdate()                     { this->update = false;                                                 }
        uint*   getGridpos()                    { return this->curcell.gridpos;                                         }
        int*    getCellpos()                    { return this->curcell.worldpos;                                        }
        DAGCI*  getCellPositions()              { return this->nearcells;                                               }

        float   getFarW()                       { return this->wFar;                                                    }
        float   getFarH()                       { return this->hFar;                                                    }
        float   getFarZ()                       { return this->zFar;                                                    }
        float   getNearZ()                      { return this->zNear;                                                   }

        float   getFOV ()                       { return this->FOV;                                                     }
        float   getOrthoScale()                 { return this->orthoScale;                                              }

    private:

        glm::mat4 projection;

        glm::vec3 pos;
        glm::vec3 fwd;
        glm::vec3 up;

        glm::vec3 altpos;

        COLFACE   planes[6];

        float     zNear;
        float     zFar;
        float     hFar;
        float     wFar;
        float     hNear;
        float     wNear;

        glm::vec3 yAxis           = glm::vec3(0, 1, 0);

        bool      update          = true;
        float     pitch           = 0.0f;
        float     yaw             = 0.0f;
        float     FOV             = 0.0f;
        float     orthoScale      = 0.0175f;

        DAGCI     curcell         = { 0 };
        DAGCI*    nearcells       = 0;
        uint      prevframe_cells = 0;

};

extern DA_CAMERA* actcam;
extern glm::mat4  actcam_viewproj;
extern glm::vec3  actcam_fwd;
extern glm::vec3  actcam_pos;

void   camlook();

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __DARKAGE_CAMERA_H__
