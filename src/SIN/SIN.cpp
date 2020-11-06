#include "SIN.h"

#include "rend/SIN_Render.h"
#include "rend/SIN_Canvas.h"
#include "rend/SIN_Ambient.h"
#include "rend/SIN_Lighting.h"

#include "shaders/SIN_DepthShader.h"
#include "shaders/SIN_CanvasShader.h"

#include "lymath/ZJC_GOPS.h"

static unsigned char INITFLAGS   = 0;

glm::vec4       ambientColor(0.0f, 0.0f, 0.0f, 1.0f);

float           ambientMult      = 1.00f;
float           ambientIntensity = 0.05f;

//  - --- - --- - --- - --- -

void SIN_INIT(ushort wWidth,
              ushort wHeight,
              unsigned char flags)              {

    SIN_Batcher_init     (                                                    );

    SIN_texbucket_init   (                                                    );
    SIN_shdbucket_init   (                                                    );
    SIN_matbucket_init   (                                                    );
    SIN_meshbucket_init  (                                                    );

    SIN_SHADOWSIZE = maxi(wWidth, wHeight) / SIN_SHADOWSCALE;

    SIN_Ambient_init     (                                                    );
    SIN_Lighting_init    (                                                    );
    SIN_DepthBuffer_init (&SIN_DepthShader                                    );
    SIN_Canvas_init      (&SIN_CanvasShader                                   );
    SIN_RenderBucket_init(                                                    );

    INITFLAGS = flags;                                                                                                  }

void SIN_END()                                  {

    SIN_RenderBucket_end();
    SIN_Canvas_end      ();
    SIN_DepthBuffer_end ();
    SIN_Lighting_init   ();
    SIN_Ambient_end     ();

    SIN_meshbucket_end  ();
    SIN_matbucket_end   ();
    SIN_shdbucket_end   ();
    SIN_texbucket_end   ();

    SIN_Batcher_end     ();                                                                                             }

//  - --- - --- - --- - --- -

void SIN_SWIPESCREEN()                          {

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor( ambientColor[0] * ambientMult,
                  ambientColor[1] * ambientMult,
                  ambientColor[2] * ambientMult,
                  ambientColor[3] * ambientMult  );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                                                                 }

void SIN_UPDATE_AMBIENT()                       { SIN_Ambient_setSky(&glm::vec3(ambientColor), ambientIntensity);       }

//  - --- - --- - --- - --- -
