#ifndef __DARKAGE_GAMEOBJECT_H__
#define __DARKAGE_GAMEOBJECT_H__

#include "spatial/ZJC_Transform.h"
#include "spatial/ZJC_Time.h"

#include "types/SIN_Mesh.h"
#include "GAOL_Constants.h"
#include "GAOL_Bounds.h"

#include "../DA_CommonTypes.h"

#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

class DA_NODE {

    public:

        DA_NODE         (ushort meshid,
                         glm::vec3 pos   = {0,0,0},
                         glm::quat rot   = {1,0,0,0},
                         glm::vec3 scale = {1,1,1});

        virtual ~DA_NODE();

//  - --- - --- - --- - --- -

        friend void DA_objects_update();
        friend void DA_objects_init  ();
        friend void DA_objects_end   ();

//  - --- - --- - --- - --- -

        int        isDynamic    ()              { return this->physMode  == GAOL_PHYSMODE_DYNAMIC;                      }
        int        isStatic     ()              { return this->physMode  == GAOL_PHYSMODE_STATIC;                       }
        glm::mat4  getModel     (bool ig)       { return transform->getModel(ig);                                       }
        glm::mat4  getNormal    ()              { return transform->getNormal(this->model);                             }
        void       setParent    (DA_NODE* other){ this->transform->setParent(other->transform);                         }
        glm::vec3& worldPosition()              { return this->transform->position;                                     }
        void       buildBounds  (glm::mat4 m)   { this->bounds->genBox(m);                                              }
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
        bool       dynOnDyn     (DA_NODE* other){ return other->isDynamic() && this->isDynamic();                       }
        bool       stcOnStc     (DA_NODE* other){ return other->isStatic()  && this->isStatic();                        }
        bool       canCol       ()              { return isStatic() || isDynamic();                                     }
        float      getSpeed     ()              { return glm::length(this->accel + this->angvel) / (fBy * 2.0f);        }
        bool       pointInside  (glm::vec3 p)   { return this->bounds->box->pointInside(p);                             }
        void       standingOn   (DA_NODE* other){ this->ground = other;                                                 }
        void       clampToSurf  ()              { this->worldPosition().y = ground->bounds->box->faces[1].centroid.y;   }
        uint*      getGridpos   ()              { return cellinfo.gridpos;                                              }
        int*       getCellpos   ()              { return cellinfo.worldpos;                                             }

//  - --- - --- - --- - --- -

        void       onCellExit   ();
        void       move         (glm::vec3 mvec, bool local = false);
        Booflo     colCheckBox  (DA_NODE* other, bool& mote);
        bool       distCheck    (DA_NODE* other, float fac = 1.25f);
        void       physicsSim   (DA_NODE* other, Booflo& groundCheck, bool& motCheck, bool& foundGround);
        void       draw         ();

//  - --- - --- - --- - --- -

    protected:

        ushort     id;

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
        bool       needsUpdate  = true;

        bool       commands[5]  = { false, false, false, false, false };

        DANCI      cellinfo     = { 0, 0, 0, 0, 0 };

//  - --- - --- - --- - --- -

    private:

        void       prePhysUpdate();

};

//  - --- - --- - --- - --- -

void DA_objects_update();
void DA_objects_init  ();
void DA_objects_end   ();

extern std::vector<DA_NODE*> SCENE_OBJECTS;

#ifdef __cplusplus
}
#endif

#endif // __DARKAGE_GAMEOBJECT_H__
