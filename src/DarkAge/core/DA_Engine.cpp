#include "DA_Engine.h"

#include "rend/SIN_Render.h"
#include "rend/SIN_Canvas.h"

#include "types/SIN_Material.h"
#include "types/SIN_Shader.h"
#include "types/SIN_Shader_EX.h"
#include "types/SIN_Texture.h"
#include "types/SIN_MeshBatch.h"

#include "../types/DA_WorldManager.h"
#include "../types/DA_Camera.h"

#include "../Chasm/wind/chMANG.h"

//  - --- - --- - --- - --- -

static ushort          render_obcount      = 0;
static ushort          DA_ACTIVE_OCCLUDERS = 0;
static ushort          total_materials     = 0;
static ushort          alpha_offset        = 0;
static SIN_LIGHT       lights[1]           = { IDENTITY, {0, 0.5, 5}, {0, 0, -1}, { 1, 1, 1, 1 } };

//  - --- - --- - --- - --- -

void SIN_Render_DepthPass()                     {

    glm::mat4 projection = getOrthographicProjection(SIN_SHADOWSIZE_X, SIN_SHADOWSIZE_Y,
                                                     actcam->getOrthoScale() * 6, actcam->getNearZ(), actcam->getFarZ());

    lights[0].mat        = projection * glm::lookAt(lights[0].position, lights[0].position + lights[0].dirn,
                                                    glm::vec3(0,1,0)                                               );

    SIN_RenderBucket_wipe                      ();
    total_materials     = SIN_getActiveMatCount();
    alpha_offset        = SIN_getOpaqueMatCount();

    DA_ACTIVE_OCCLUDERS = 0;
    render_obcount      = 0;

//  - --- - --- - --- - --- -

    uint  depthProgram = SIN_DepthBuffer_getProgram();
    uint* depthUniform = SIN_DepthBuffer_getUniform();

    glUseProgram        (depthProgram                                            );
    glUniformMatrix4fv  (depthUniform[1], 1, GL_FALSE, &((lights[0].mat)[0][0])  );
    SIN_DepthBuffer_bind(                                                        );
    SIN_DepthMap_bind   (                                                        );
    glCullFace          (GL_FRONT                                                );

//  - --- - --- - --- - --- -

    for(uint i = 0; i < FRUSTUM_OBJECTS; i++)
    {

        DA_NODE* ob = SCENE_OBJECTS[FRAME_OBJECTS[i]];

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
                    ob->visible    = true;
                    Material* mate = SIN_matbucket_get(ob->mesh->matloc);
                    uint jstart    = 0;
                    if(!mate->opaque) { jstart = alpha_offset; }

                    uint mat_index = 0;
                    for(uint j = jstart; j < total_materials; j++)
                    {
                        if     (SIN_RenderBucket[total_materials][j] < 0)
                        { SIN_RenderBucket[total_materials][j] = ob->mesh->matloc; mat_index = j; break;   }

                        else if(SIN_RenderBucket[total_materials][j] == ob->mesh->matloc)
                        { mat_index = j; break;                                                         }
                    }

//  - --- - --- - --- - --- -

                    glUniformMatrix4fv(depthUniform[0], 1, GL_FALSE, &((ob->model)[0][0]));
                    chkbatch          (ob->mesh->drawLoc                                 );
                    draw_mesh         (ob->mesh                                          );

                    render_obcount++;

                    uint draw_index = SIN_RenderBucket[mat_index][0];

                    SIN_RenderBucket[mat_index][draw_index + 1]  = ob->id;
                    SIN_RenderBucket[mat_index][0]              += 1;

                    if(ob->isOccluder())
                    {
                        ob->occlu->getFrustum  ( actcam_fwd,
                                                 actcam_pos,
                                                 actcam->getUp(),
                                                 actcam->getFarZ(),
                                                 actcam->getFarH(),
                                                 actcam->getFarW()  );

                        SCENE_OCCLUDERS[DA_ACTIVE_OCCLUDERS] = ob->id;
                        DA_ACTIVE_OCCLUDERS++;
                    }
                }
            }
        }
    }

    glCullFace      (GL_BACK);
    shader_reset_loc(       );                                                                                          }

//  - --- - --- - --- - --- -

void SIN_Render_ColorPass()                     {

    if(!render_obcount) { return; }

    glViewport(0, 0, getWinWidth(), getWinHeight());

    // SIN_Canvas_draw  ();
    // glClear(GL_DEPTH_BUFFER_BIT);

    for(uint mat_index = 0; mat_index < total_materials; mat_index++)
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
                    mate           = SIN_matbucket_get(ob->mesh->matloc);
                    int shaderSwap = shader_chkProgram(mate->shdloc    );

                    if(shaderSwap || actcam->getUpdate())
                    {
                        shader_update_camera(&actcam_viewproj, &actcam_fwd, &actcam_pos);
                        shader_update_lights(lights, 1);
                        SIN_DepthMap_bind();
                    }

                    for(uchar t = 0; t < mate->num_textures; t++)
                    { bind_tex_to_slot(mate->texloc[t], SIN_TEXID_BASE + t); }
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
                    mate           = SIN_matbucket_get(ob->mesh->matloc);
                    int shaderSwap = shader_chkProgram(mate->shdloc    );

                    if(shaderSwap || actcam->getUpdate())
                    {
                        shader_update_camera(&actcam_viewproj, &actcam_fwd, &actcam_pos);
                        shader_update_lights(lights, 1);
                        SIN_DepthMap_bind();
                    }

                    for(uchar t = 0; t < mate->num_textures; t++)
                    { bind_tex_to_slot(mate->texloc[t], SIN_TEXID_BASE + t); }
                }

                chkbatch(ob->mesh->drawLoc); ob->draw();
            }
        }
    }

    actcam->endUpdate();                                                                                                }

//  - --- - --- - --- - --- -

