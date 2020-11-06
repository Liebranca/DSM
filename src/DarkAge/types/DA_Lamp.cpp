#include "DA_Lamp.h"
#include "DA_Camera.h"

#include "SIN_Constants.h"

//  - --- - --- - --- - --- -

DA_LAMP::DA_LAMP (ushort meshid, uint flags,

                  glm::vec3 mpos,
                  glm::quat mrot,
                  glm::vec3 mscale,

                  glm::vec3 lpos,
                  glm::vec4 color,
                  glm::vec3 ldirn,

                  float reach,
                  float zNear,
                  float zFar             )

                 :DA_NODE(meshid, flags,
                          mpos, mrot, mscale)   {

    lamp          = new SIN_LAMP();

    lamp->mat     = IDENTITY;            light_position = lpos;
    light_dir     = glm::vec4(ldirn, 1); light_color    = color;

    frustum       = new viewFrustum(glm::vec2(SIN_SHADOWSIZE, SIN_SHADOWSIZE), 90.0f, 1,
                                    SIN_ORTHOSCALE * SIN_SHADOWSCALE_H, zNear, zFar      );                             }

//  - --- - --- - --- - --- -

void DA_LAMP::onUpdate()                        { DA_NODE::onUpdate();

    glm::vec3 worldSpacePosition = light_worldPosition              (                                           );
    glm::vec3 worldSpaceFwd      = light_worldPosition              (                                           );

    lamp->mat                    = frustum->projection * glm::lookAt(worldSpacePosition,
                                                                     worldSpacePosition + worldSpaceFwd,
                                                                     yAxis                                      );

    if( (fabs(worldSpaceFwd.y)  != 0)
    &&  (worldSpaceFwd.x        == 0)
    &&  (worldSpaceFwd.z        == 0) )                             { worldSpaceFwd.z += 0.01f;                 }

    glm::vec3 hAxis              = glm::normalize                   (glm::cross(yAxis,         worldSpaceFwd   ));
    glm::vec3 up                 = glm::normalize                   (glm::cross(worldSpaceFwd, hAxis           ));
    frameUpdated                 = 1;

    lamp->position               = glm::vec4                        (worldSpacePosition,       reach            );
    lamp->dir                    = glm::vec4                        (worldSpaceFwd,            1                );

    lamp->color                  = light_color;

    frustum->buildPlanes(worldSpacePosition, worldSpaceFwd, up);                                                        }

//  - --- - --- - --- - --- -

