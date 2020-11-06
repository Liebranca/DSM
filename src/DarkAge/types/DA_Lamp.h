#ifndef __DARKAGE_LAMP_H__
#define __DARKAGE_LAMP_H__

#include "DA_GameObject.h"
#include "GAOL_Frustum.h"

#include "rend/SIN_Lighting.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

class DA_LAMP: public DA_NODE {

    public:

        DA_LAMP (ushort meshid, uint flags,

                 glm::vec3 mpos   = { 0, 0, 0   },
                 glm::quat mrot   = { 1, 0, 0, 0},
                 glm::vec3 mscale = { 1, 1, 1   },

                 glm::vec3 lpos   = { 0, 0, 0   },
                 glm::vec4 color  = { 1, 1, 1, 1},
                 glm::vec3 ldirn  = { 0,-1, 0   },

                 float reach      =  8.00f,
                 float zNear      =  0.01f,
                 float zFar       = 100.0f         );

        virtual ~DA_LAMP()                      { delete frustum; delete lamp;                                          }

//  - --- - --- - --- - --- -

        viewFrustum*  getFrustum      ()        { return frustum;                                                       }
        SIN_LAMP*     getLight        () const  { return lamp;                                                          }
        bool          updatedLastFrame()        { return frameUpdated;                                                  }

//  - --- - --- - --- - --- -

    protected:

        void         onUpdate        ();
        void         endUpdate       ()         { frameUpdated = 0;                                                     }

//  - --- - --- - --- - --- -

    private:

        glm::vec3 light_worldPosition() const   { return glm::vec4(light_position, 1) * model;                          }
        glm::vec3 light_worldDirn    () const   { return light_dir                    * nmat;                           }

//  - --- - --- - --- - --- -

        glm::vec3     light_position;
        glm::vec3     light_dir;
        glm::vec4     light_color;

        float reach;

        SIN_LAMP*     lamp;
        viewFrustum*  frustum;

        bool          frameUpdated = 0;

};

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif //__DARKAGE_LAMP_H__
