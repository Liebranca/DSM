#ifndef __DARKAGE_GAMEOBJECT_H__
#define __DARKAGE_GAMEOBJECT_H__

#include "ZJC_CommonTypes.h"
#include "spatial/ZJC_Transform.h"
#include "spatial/ZJC_Time.h"

#include "types/SIN_Mesh.h"
#include "GAOL_Constants.h"
#include "GAOL_Bounds.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

class DA_NODE {

    public:

        DA_NODE(ushort meshid,
                glm::vec3 pos   = {0,0,0},
                glm::quat rot   = {1,0,0,0},
                glm::vec3 scale = {1,1,1});

        virtual ~DA_NODE();

//  - --- - --- - --- - --- -

        glm::mat4  getModel     (bool ig)       { return transform->getModel(ig);                                       }
        glm::mat4  getNormal    ()              { return transform->getNormal(this->model);                             }
        void       setParent    (DA_NODE* other){ this->transform->setParent(other->transform);                         }
        glm::vec3& worldPosition()              { return this->transform->position;                                     }
        void       buildBounds  ()              { this->bounds->genBox(this->model);                                    }
        bool       isOnGround   ()              { return this->ground != nullptr;                                       }
        bool       allowJump    ()              { return this->isOnGround() && !this->isJumping;                        }
        bool       isMapNode    ()              { return (this->nodeFlags & 1);                                         }
        bool       isMoving     ()              { return (this->hasAccel || this->hasRot);                              }
        DA_NODE*   getGround    ()              { return this->ground;                                                  }
        DA_NODE*   getTarget    ()              { return this->target;                                                  }
        void       setTarget    (DA_NODE* other){ this->target = other;                                                 }
        void       removeTarget ()              { this->target = nullptr;                                               }
        bool       groundCheck  ()              { return this->bounds->box->groundCheck( this->ground->bounds->box,
                                                                                       -10.0f).a;                       }
        bool       dynOnDyn     (DA_NODE* other){ return (  (other->physMode == GAOL_PHYSMODE_DYNAMIC)
                                                         && (this->physMode  == GAOL_PHYSMODE_DYNAMIC));                }
        bool       stcOnStc     (DA_NODE* other){ return (  (other->physMode == GAOL_PHYSMODE_STATIC)
                                                         && (this->physMode  == GAOL_PHYSMODE_STATIC));                 }
        bool       canCol       ()              { return (  (this->physMode  == GAOL_PHYSMODE_STATIC)
                                                         || (this->physMode  == GAOL_PHYSMODE_DYNAMIC));                }
        float      getSpeed     ()              { return glm::length(this->accel + this->angvel) / (fBy * 2.0f);        }
        bool       pointInside  (glm::vec3 p)   { return this->bounds->box->pointInside(p);                             }
        void       standingOn   (DA_NODE* other){ this->ground = other;                                                 }
        void       clampToSurf  ()              { this->worldPosition().y = ground->bounds->box->faces[1].centroid.y;   }

//  - --- - --- - --- - --- -

        Booflo     colCheckBox  (DA_NODE* other, bool& mote);
        bool       distCheck    (DA_NODE* other, float fac = 1.25f);
        void       physicsSim   (DA_NODE* other, Booflo& groundCheck, bool& motCheck, bool& foundGround);
        bool       draw         ();

//  - --- - --- - --- - --- -

    protected:

        T3D*       transform;
        COLBOUNDS* bounds;
        M3D*       mesh;

        DA_NODE*   target;
        DA_NODE*   ground;

        glm::mat4  model;
        glm::mat3  nmat;

        glm::vec3  accel        = { 0,0,0 };
        glm::vec3  angvel       = { 0,0,0 };
        glm::vec3  vel          = { 0,0,0 };

        float      speedmult    = 10.0f;
        float      weight       = 1.0f;
        bool       isJumping    = false;
        bool       hasAccel     = false;
        bool       hasRot       = false;

        uint       nodeFlags    = 0;
        uint       physMode     = 0;
        uint       momentum     = 0;
        uint       max_momentum = 200;

        bool       visible      = false;
        bool       doRender     = true;

        bool       commands[5]  = { false, false, false, false, false };

//  - --- - --- - --- - --- -

    private:

        

};

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __DARKAGE_GAMEOBJECT_H__
