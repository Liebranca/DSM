#include "DA_GameObject.h"
#include "DA_WorldManager.h"
#include "DA_Camera.h"

#include "lyutils/ZJC_Evil.h"
#include "lymath/ZJC_GOPS.h"
#include "lymath/ZJC_VOPS.h"
#include "lyarr/ZJC_Stack.h"

#include "types/SIN_Shader_EX.h"

#define               DA_MAX_OBJECTS      1024

static sStack*        DA_OBJ_SLOTSTACK  = NULL;
static ushort         DA_ACTIVE_OBJECTS = 0;

std::vector<DA_NODE*> SCENE_OBJECTS(DA_MAX_OBJECTS, 0);

//  - --- - --- - --- - --- -

void DA_objects_init()                          {

    DA_OBJ_SLOTSTACK = build_sStack(DA_MAX_OBJECTS);
    for(int i = DA_MAX_OBJECTS-1; i > 0; i--)   { sStack_push(DA_OBJ_SLOTSTACK, i);                                     }
                                                                                                                        }

void DA_objects_end()                           {

    for(uint i = 0; i < DA_MAX_OBJECTS; i++)    { DA_NODE* ob = SCENE_OBJECTS[i];
                                                  if(ob) { delete ob; }                                                 }

    del_sStack(DA_OBJ_SLOTSTACK);                                                                                       }

//  - --- - --- - --- - --- -

DA_NODE::DA_NODE(ushort meshid,
                 glm::vec3 pos,
                 glm::quat rot,
                 glm::vec3 scale)               {

    transform       = new T3D(pos, rot, scale);
    mesh            = SIN_meshbucket_find(meshid);
    ushort meshloc  = SIN_meshbucket_findloc(meshid);

    if(mesh->bounds != NULL)
    {

        glm::vec4 points[8];

        for(int i = 0; i < 8; i++)
        {
            points[i] = { frac8_tofloat(mesh->bounds[i].co[0]),
                          frac8_tofloat(mesh->bounds[i].co[0]),
                          frac8_tofloat(mesh->bounds[i].co[0]),

                          1                                             };

        }

        GAOL_boundbucket_add(points, meshloc);
        WARD_EVIL_MFREE     (mesh->bounds);

    }

    mesh->users++;

    bounds   = GAOL_boundbucket_get(meshloc);
    ushort obloc = sStack_pop(DA_OBJ_SLOTSTACK);
    WARD_EVIL_UNSIG(obloc, 1);

    SCENE_OBJECTS[obloc] = this;
    DA_ACTIVE_OBJECTS++;

    float fpos[3] = { pos.x, pos.y, pos.z };
    DA_grid_findpos(this->gridpos, fpos);

    this->id     = obloc;
    DA_grid_regObject(gridpos, cellinfo, obloc, isDynamic());                                                           }

DA_NODE::~DA_NODE()                             {

    delete this->transform;
    unsub_mesh (SIN_meshbucket_findloc(this->mesh->id));
    sStack_push(DA_OBJ_SLOTSTACK, this->id);                                                                            }

//  - --- - --- - --- - --- -

void DA_NODE::physicsSim(DA_NODE* other,
                         Booflo& groundCheck,
                         bool& motCheck,
                         bool& foundGround)     {

    if (!other->bounds->box)                    { return;                                                               }

    if (other->canCol() && this->distCheck(other))
    {
        groundCheck      = this->colCheckBox(other, motCheck);
        bool groundCondo = (   (!foundGround) && (groundCheck.a && !this->isJumping)
                           && !(groundCheck.x > this->worldPosition().y + 0.5f)      );

        if (groundCondo)                        { this->standingOn(other); this->clampToSurf();                         }
    }
                                                                                                                        }

Booflo DA_NODE::colCheckBox(DA_NODE* other,
                            bool& mote)         {

    glm::vec3 dirn = this->accel * 10.0f;

    COLBOX proj    = this->bounds->box->selfProjection(dirn);
    glm::vec3 n    = proj.boxToBox(other->bounds->box, glm::normalize(dirn));

    if (n.x || n.y || n.z)
    {
        n           =                           n * fBy * 5.5f;
        bool iang[] =                           { true, true, true                                                      };
                
        if (this->stcOnStc(other))              { iang[0] = false; iang[1] = false; iang[2] = false;                    }

        if (n.x)
        {
            mote         = false;
            other->vel.x = -n.x * other->weight;

            if (this->accel.x)                  { this->vel.x = n.x;                                                    }
            if (!iang[2])                       { this->angvel.z *= -1; iang[2] = true;                                 }
            if (!iang[1])                       { this->angvel.y *= -1; iang[1] = true;                                 }
        }

        if (n.y)
        {
            other->vel.y = -n.y * other->weight;

            if (  (this->accel.y)
               && (other != this->ground) )     { this->vel.y = n.y;                                                    }
            if (!iang[0])                       { this->angvel.x *= -1; iang[0] = true;                                 }
            if (!iang[2])                       { this->angvel.z *= -1; iang[2] = true;                                 }
        }

        if (n.z)
        {
            mote         = false;
            other->vel.z = -n.z * other->weight;

            if (this->accel.z)                  { this->vel.z = n.z;                                                    }
            if (!iang[0])                       { this->angvel.x *= -1; iang[0] = true;                                 }
            if (!iang[1])                       { this->angvel.y *= -1; iang[1] = true;                                 }
        }

        if (  other->physMode
           == GAOL_PHYSMODE_STATIC)             { return proj.groundCheck(other->bounds->box, -10.0f);                  }
    }

    return { false, 0.0f };                                                                                             }

bool DA_NODE::distCheck(DA_NODE* other,
                        float fac)              {

    float l1 = other->bounds->box->area[0] + this->bounds->box->area[0];
    float l2 = 0.0f;

    float yd = subminf(fabs(this->worldPosition().y), fabs(other->worldPosition().y));

    bool  e1 = glm::distance(xzvec(other->worldPosition()), xzvec(this->worldPosition()) ) < l1/fac;

    if ( this->worldPosition().y
       < other->worldPosition().y)              { l2 = this->bounds->box->area[1];                                      }

    else                                        { l2 = other->bounds->box->area[1];                                     }

    bool  e2 = yd < l2;
    return e1 && e2;                                                                                                    }

//  - --- - --- - --- - --- -

void DA_NODE::onCellExit()                      {

    DA_grid_unregObject(cellinfo, isDynamic());
    DA_NODE* other = SCENE_OBJECTS[cellinfo[1]];
    other->cellinfo[0] = cellinfo[0];
    }

void DA_NODE::move      (glm::vec3 mvec,
                         bool local)            {

    worldPosition() += mvec;
    needsUpdate      = true;                                                                                            }

bool DA_NODE::draw() {

    if(needsUpdate)                             { model       = transform->getModel();
                                                  nmat        = transform->getNormal(model);
                                                  needsUpdate = false;

                                                  buildBounds();                                                        }

    if(doRender)
    {

        /*for (unsigned int i = 0; i < this->children.size(); i++) {
            this->children[i]->worldPosition() = this->mesh->po_xyz[i];
            this->children[i]->worldOrientation() = this->mesh->po_rot[i];
            this->children[i]->draw();
        }*/

        if(actcam->rectInFrustum(bounds->box->points))
        {

            // TODO: handle ANS here

            shader_update_model(&model);
            shader_update_nmat (&nmat );
            draw_mesh          (mesh  );

            return true;
        }

        // else{ printf("Cube did not get drawn\n"); }
    }

    return false;                                                                                                       }
