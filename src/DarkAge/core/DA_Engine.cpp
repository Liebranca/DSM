#include "DA_Engine.h"

#include "rend/SIN_Render.h"
#include "rend/SIN_Canvas.h"
#include "rend/SIN_Lighting.h"
#include "rend/SIN_Ambient.h"

#include "types/SIN_Material.h"
#include "types/SIN_Shader.h"
#include "types/SIN_Shader_EX.h"
#include "types/SIN_Texture.h"
#include "types/SIN_MeshBatch.h"

#include "../types/DA_WorldManager.h"
#include "../types/DA_Camera.h"
#include "../types/DA_Lamp.h"

#include "../Chasm/wind/chMANG.h"

//  - --- - --- - --- - --- -

static ushort               render_obcount                         = 0;
static ushort               DA_ACTIVE_OCCLUDERS                    = 0;
static ushort               alpha_offset                           = 0;

static ushort               FRAME_NUM_MATERIALS                    = 0;
static uchar                FRAME_NUM_LAMPS                        = 0;
static uchar                PREVFRAME_NUM_LAMPS                    = 0;
static uchar                FRAME_DEPTHUPDATE                      = 0;

static uint                 DA_OBJECT_UPDATE_NUMOBJS               = 0;
static uint                 DA_OBJECT_UPDATE_NUMCELLS              = 0;
static ushort               FRAME_NUM_OBJECTS                      = 0;

static ushort               FRAME_LAMPS        [SIN_MAX_LAMPS    ] = {0};

static ushort               PREVFRAME_LAMPS    [SIN_MAX_LAMPS    ] = {0};
static ushort               DA_OBJECT_LOCATIONS[DA_MAX_OBJECTS   ];

static viewFrustum*         DA_LAMP_FRUSTUMS   [SIN_MAX_LAMPS    ];


static std::vector<ushort>  FRAME_OBJECTS      (DA_MAX_OBJECTS, 0);
static std::vector<ushort>  SCENE_OCCLUDERS    (DA_MAX_OBJECTS, 0);

//  - --- - --- - --- - --- -

void SIN_Render_Update()                        {

    DA_OBJECT_UPDATE_NUMOBJS  = 0;
    DA_OBJECT_UPDATE_NUMCELLS = 0;
    FRAME_NUM_OBJECTS         = 0;
    FRAME_NUM_LAMPS           = 0;
    FRAME_DEPTHUPDATE         = 0;

    for(uchar i = 0; i < SIN_MAX_LAMPS; i++) { FRAME_LAMPS[i] = 0; }

    actcam->resetCulling();

    DA_grid_fetchOblocs(actcam->getCellpos(),
                        actcam->getCellPositions(),
                        &DA_OBJECT_UPDATE_NUMCELLS,
                        &DA_OBJECT_UPDATE_NUMOBJS,
                        DA_OBJECT_LOCATIONS        );

//  - --- - --- - --- - --- -

    if(actcam->getUpdate())                     { actcam_viewproj = actcam->getViewProjection();
                                                  actcam_fwd      = actcam->getFwd();
                                                  actcam_pos      = actcam->getPos();

                                                  SIN_Ambient_setCam(&actcam_viewproj, &actcam_fwd);

                                                  actcam->updateFrustum();                                              }

//  - --- - --- - --- - --- -

    uint k = 0;
    for(uint i = 0; i < DA_OBJECT_UPDATE_NUMOBJS; i++)
    {
        ushort    closest  = 0;
        int       isLight  = 0;
        float     dist     = 520200;
        float     new_dist = 520200;

        for(uint j = 0; j < DA_OBJECT_UPDATE_NUMOBJS; j++)
        {
            if(DA_OBJECT_LOCATIONS[j])
            {
                DA_NODE* ob = SCENE_OBJECTS[DA_OBJECT_LOCATIONS[j]];
                isLight     = ob->isLightSource();

                if(ob->needsUpdate) { ob->onUpdate(); }

                new_dist    = glm::distance(actcam_pos, ob->transform->position);
                if(new_dist < dist)             { dist = new_dist; closest = j+1;                                       }
            }
        }

//  - --- - --- - --- - --- -

        if(closest)
        {

            ushort obid                      = DA_OBJECT_LOCATIONS[closest-1];

            FRAME_OBJECTS[FRAME_NUM_OBJECTS] = obid;
            DA_OBJECT_LOCATIONS[closest-1]   = 0;
            FRAME_NUM_OBJECTS++;

            DA_NODE* ob                      = SCENE_OBJECTS[obid];

//  - --- - --- - --- - --- -

            if(isLight && FRAME_NUM_LAMPS < SIN_MAX_LAMPS)
            {

                DA_LAMP* lamp = (DA_LAMP*) ob;

                if(actcam->frustumIsect(lamp->getFrustum()))
                {

                    int wasInPrev = 0;

                    for(uchar i = 0; i < PREVFRAME_NUM_LAMPS; i++)
                    { if(PREVFRAME_LAMPS[i] == obid) { wasInPrev = 1; break; } }

                    if(!wasInPrev || lamp->updatedLastFrame())
                    { SIN_Lighting_setDir(lamp->getLight(), FRAME_NUM_LAMPS); FRAME_DEPTHUPDATE = 1; }

                    DA_LAMP_FRUSTUMS[FRAME_NUM_LAMPS] = lamp->getFrustum();
                    FRAME_LAMPS     [FRAME_NUM_LAMPS] = obid;

                    FRAME_NUM_LAMPS++;
                }
            }

//  - --- - --- - --- - --- -

            ob->endUpdate();
        }
    }

    if(FRAME_NUM_LAMPS != PREVFRAME_NUM_LAMPS)
    { SIN_Lighting_setNum(FRAME_NUM_LAMPS); }

    SIN_Lighting_updateCheck();
    SIN_Ambient_updateCheck ();

    if(!FRAME_DEPTHUPDATE) { FRAME_DEPTHUPDATE = actcam->getUpdate(); }

    PREVFRAME_NUM_LAMPS = FRAME_NUM_LAMPS;
    for(uchar i = 0; i < FRAME_NUM_LAMPS; i++) { PREVFRAME_LAMPS[i] = FRAME_LAMPS[i]; }

    actcam->cellCulling(DA_OBJECT_UPDATE_NUMCELLS, DA_LAMP_FRUSTUMS, FRAME_NUM_LAMPS);                                  }

//  - --- - --- - --- - --- -

void SIN_Render_DepthPass()                     {

    SIN_RenderBucket_wipe                      ();
    FRAME_NUM_MATERIALS = SIN_getActiveMatCount();
    alpha_offset        = SIN_getOpaqueMatCount();

    DA_ACTIVE_OCCLUDERS = 0;
    render_obcount      = 0;

//  - --- - --- - --- - --- -

    uint  depthProgram = SIN_DepthBuffer_getProgram();
    uint* depthUniform = SIN_DepthBuffer_getUniform();

    glUseProgram        (depthProgram                                            );
    
    SIN_DepthBuffer_bind(                                                        );
    SIN_DepthMap_bind   (                                                        );
    glCullFace          (GL_FRONT                                                );

//  - --- - --- - --- - --- -

    for(uint i = 0; i < FRAME_NUM_OBJECTS; i++)
    {

        DA_NODE* ob = SCENE_OBJECTS[FRAME_OBJECTS[i]];

        for(uchar j = 0; j < FRAME_NUM_LAMPS * FRAME_DEPTHUPDATE; j++)
        {
            if(DA_grid_getInLightFrustum(ob->cellinfo.gridpos, j))
            {

                DA_LAMP* lamp = (DA_LAMP*) SCENE_OBJECTS[FRAME_LAMPS[j]];

                if(lamp->getFrustum()->sphereIsect(ob->bounds->sphere))
                {
                    glUniformMatrix4fv(depthUniform[0], 1, GL_FALSE, &((ob->model            )[0][0]));
                    glUniform1i       (depthUniform[1], j                                            );
                    chkbatch          (ob->mesh->drawLoc                                             );
                    SIN_drawMesh      (ob->mesh                                                      );
                }
            }
        }

//  - --- - --- - --- - --- -

        if(DA_grid_getInFrustum(ob->cellinfo.gridpos))
        {
            if(actcam->sphInFrustum(ob->bounds->sphere))
            {

                int eyechk = 1;

                for(uint occlu_id = 0; occlu_id < DA_ACTIVE_OCCLUDERS; occlu_id++)
                {
                    DA_OCCLUDER* occlu = SCENE_OBJECTS[occlu_id]->occlu;
                    eyechk             = occlu->sphTest(ob->bounds->sphere);

                    if(eyechk < 0) { eyechk = occlu->getVisible(ob->bounds->box->points); }
                    if(!eyechk)    { break;                                               }
                }

//  - --- - --- - --- - --- -

                if(eyechk)
                {
                    ob->visible      = true;

                    Material* mate   = SIN_matbucket_get(ob->mesh->matloc);
                    uint      jstart = 0;

                    if(!mate->opaque) { jstart = alpha_offset; }

                    uint mat_index = 0;
                    for(uint j = jstart; j < FRAME_NUM_MATERIALS; j++)
                    {
                        if     (SIN_RenderBucket[SIN_RenderBucket_size][j] < 0)
                        { SIN_RenderBucket[SIN_RenderBucket_size][j] = ob->mesh->matloc; mat_index = j; break;   }

                        else if(SIN_RenderBucket[SIN_RenderBucket_size][j] == ob->mesh->matloc)
                        { mat_index = j; break;                                                         }
                    }

//  - --- - --- - --- - --- -

                    render_obcount++;

                    uint draw_index = SIN_RenderBucket[mat_index][0];

                    SIN_RenderBucket[mat_index][draw_index + 1]  = ob->id;
                    SIN_RenderBucket[mat_index][0]              += 1;

                    if(ob->isOccluder())
                    {
                        ob->occlu->getFrustum  ( actcam_fwd,
                                                 actcam_pos,
                                                 actcam->getUp(),
                                                 actcam->getFrustum()->zFar,
                                                 actcam->getFrustum()->hFar,
                                                 actcam->getFrustum()->wFar  );

                        SCENE_OCCLUDERS[DA_ACTIVE_OCCLUDERS] = ob->id;
                        DA_ACTIVE_OCCLUDERS++;
                    }
                }
            }
        }
    }

    glCullFace      (GL_BACK);
    SIN_resetShdloc (       );                                                                                          }

//  - --- - --- - --- - --- -

void SIN_Render_ColorPass()                     {

    if(!render_obcount) { return; }

    glViewport(0, 0, getWinWidth(), getWinHeight());

    for(uint mat_index = 0; mat_index < FRAME_NUM_MATERIALS; mat_index++)
    {

        Material* mate = NULL;

        if(mat_index < alpha_offset)
        {

            int istart = 1;
            int iend   = SIN_RenderBucket[mat_index][0] + 1;

            for(int draw_index = istart; draw_index < iend; draw_index++)
            {
                DA_NODE* ob = SCENE_OBJECTS[SIN_RenderBucket[mat_index][draw_index]];

                if(draw_index == istart)
                {
                    mate       = SIN_matbucket_get(ob->mesh->matloc);

                    if(SIN_chkProgram(mate->shdloc) && FRAME_DEPTHUPDATE) { SIN_DepthMap_bind(); }
                    SIN_bindTexture  (mate->texloc,    SIN_TEXID_BASE     );
                }

                chkbatch(ob->mesh->drawLoc); ob->draw();
            }
        }

//  - --- - --- - --- - --- -

        else
        {
            int istart = SIN_RenderBucket[mat_index][0] + 1;
            int iend   = 0;

            for(int draw_index = istart; draw_index > iend; draw_index--)
            {
                DA_NODE* ob = SCENE_OBJECTS[SIN_RenderBucket[mat_index][draw_index]];

                if(draw_index == istart)
                {
                    mate       = SIN_matbucket_get(ob->mesh->matloc);

                    if(SIN_chkProgram(mate->shdloc) && FRAME_DEPTHUPDATE) { SIN_DepthMap_bind(); }
                    SIN_bindTexture  (mate->texloc,    SIN_TEXID_BASE     );
                }

                chkbatch(ob->mesh->drawLoc); ob->draw();
            }
        }
    }

    /*glClear(GL_DEPTH_BUFFER_BIT);
    SIN_Canvas_draw  (SIN_texbucket_find(1)->location);*/

    actcam->endUpdate();                                                                                                }

//  - --- - --- - --- - --- -

