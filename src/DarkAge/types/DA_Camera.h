#ifndef __DARKAGE_CAMERA_H__
#define __DARKAGE_CAMERA_H__

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "SIN_Constants.h"
#include "GAOL_Frustum.h"

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

        glm::mat4 getViewProjection() const     { return frustum->projection * glm::lookAt(pos, pos + fwd, yAxis);      }
        inline glm::vec3 getEye()               { return glm::normalize(pos + fwd);                                     }

        void updateFrustum  ();

        void onAreaChange   ();
        void cellCulling    (uint num_cells, viewFrustum* lightFrustums[SIN_MAX_LAMPS], uint num_dirlights);
        void resetCulling   ();

        int  sphInFrustum   (COLSPHERE* sph);
        bool pointsInFrustum(glm::vec3* points, uint num_points);
        bool pointInFrustum (glm::vec3& point);
        bool cageInFrustum  (COLBOX*    box);

        int frustumIsect    (viewFrustum* other);

        void projOrtho      (float width, float height);
        void projPersp      (float width, float height);

        void move           (glm::vec3 mvec, bool local = false);
        void rotate         (glm::vec3 rvec);
        
        float      getPitch ()                  { return pitch;                                                         }
        glm::vec3  getFwd   ()                  { return fwd;                                                           }
        glm::vec3& getPos   ()                  { return pos;                                                           }
        glm::vec3& getUp    ()                  { return up;                                                            }
        glm::vec3 getFwdCast(float dist = 3.5f) { return pos + (fwd * dist);                                            }
        void       snapTo   (glm::vec3 newpos)  { this->altpos = this->pos;
                                                  this->pos = newpos;                                                   }
        void    undoSnap    ()                  { this->pos = this->altpos;                                             }
        bool    getUpdate   ()                  { return this->update;                                                  }
        void    endUpdate   ()                  { this->update = false;                                                 }
        uint*   getGridpos  ()                  { return this->curcell.gridpos;                                         }
        int*    getCellpos  ()                  { return this->curcell.worldpos;                                        }
        DAGCI*  getCellPositions()              { return this->nearcells;                                               }

        viewFrustum* getFrustum()               { return this->frustum;                                                 }

    private:

        glm::vec3    pos;
        glm::vec3    fwd;
        glm::vec3    up;

        glm::vec3    altpos;
        viewFrustum* frustum;

        bool         update          = true;
        float        pitch           = 0.0f;
        float        yaw             = 0.0f;

        DAGCI        curcell         = { 0 };
        DAGCI*       nearcells       = 0;
        uint         prevframe_cells = 0;

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
