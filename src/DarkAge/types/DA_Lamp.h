#ifndef __DARKAGE_LAMP_H__
#define __DARKAGE_LAMP_H__

#include "DA_GameObject.h"

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

                 glm::vec4 lpos   = { 0, 0, 0, 1},
                 glm::vec4 color  = { 1, 1, 1, 1},
                 glm::vec3 ldirn  = { 0,-1, 0   },

                 float     rad    = 6.0f,
                 float     atte   = 1.0f        )

                :DA_NODE(meshid, flags, mpos, mrot, mscale),

                 pos (lpos),  color (color),
                 dirn(ldirn), radius(rad), atten(atte)      { ;                                                         }

        virtual ~DA_LAMP()                                  { ;                                                         }

//  - --- - --- - --- - --- -

        glm::vec3 worldPosition() const                     { return pos * model;                                       }

//  - --- - --- - --- - --- -

    private:

        glm::vec4 pos;
        glm::vec4 color;
        glm::vec3 dirn;

        float     radius;
        float     atten;

};

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif //__DARKAGE_LAMP_H__
