#ifndef __DARKAGE_CAMERA_H__
#define __DARKAGE_CAMERA_H__

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "GAOL_Face.h"

#include "../DA_CommonTypes.h"

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

class DA_CAMERA {

    public:

        DA_CAMERA(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar);

        virtual ~DA_CAMERA();

        glm::mat4 getViewProjection() const     { return perspective * glm::lookAt(pos, pos+fwd, yAxis);                }
        inline glm::vec3 getEye()               { return glm::normalize(pos + fwd);                                     }

        void getFrustum();
        void drawBounds(glm::vec3 bounds[8]);

        void onAreaChange  ();
        void cellCulling   (uint num_cells);
        void resetCulling  ();
        bool rectInFrustum (glm::vec3  bounds[8]);
        bool pointInFrustum(glm::vec3 point);

        void move(glm::vec3 mvec, bool local = false);
        void rotate(glm::vec3 rvec);
        
        float getPitch()                        { return pitch;                                                         }
        glm::vec3& getFwd()                     { return fwd;                                                           }
        glm::vec3& getPos()                     { return pos;                                                           }
        glm::vec3 getFwdCast(float dist = 3.5f) { return pos + (fwd * dist);                                            }
        void snapTo(glm::vec3 newpos)           { this->altpos = this->pos;
                                                  this->pos = newpos;                                                   }
        void    undoSnap()                      { this->pos = this->altpos;                                             }
        bool    getUpdate()                     { return this->update;                                                  }
        void    endUpdate()                     { this->update = false;                                                 }
        uint*   getGridpos()                    { return this->curcell.gridpos;                                         }
        int*    getCellpos()                    { return this->curcell.worldpos;                                        }
        DAGCI*  getCellPositions()              { return this->nearcells;                                               }

    private:

        glm::mat4 perspective;

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
        float     pitch           = 0;
        float     yaw             = 0;

        DAGCI     curcell         = { 0, 0, 0, 0 };
        DAGCI*    nearcells       = 0;
        uint      prevframe_cells = 0;

};

extern DA_CAMERA* actcam;
extern glm::mat4  actcam_viewproj;
extern glm::vec3  actcam_fwd;
extern glm::vec3  actcam_pos;

void caminit();

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __DARKAGE_CAMERA_H__
