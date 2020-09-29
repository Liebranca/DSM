#include "SIN.h"

#include "rend/SIN_Render.h"
#include "rend/SIN_Canvas.h"
#include "shaders/SIN_DepthShader.h"
#include "shaders/SIN_CanvasShader.h"

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

    SIN_DepthBuffer_init (SIN_DepthShader_source_v,  SIN_DepthShader_source_p );
    SIN_Canvas_init      (SIN_CanvasShader_source_v, SIN_CanvasShader_source_p);
    SIN_RenderBucket_init(                                                    );

    INITFLAGS = flags;                                                                                                  }

void SIN_END()                                  {

    SIN_RenderBucket_end();
    SIN_Canvas_end      ();
    SIN_DepthBuffer_end ();

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

void SIN_UPDATE_AMBIENT()                       { glm::vec4 ambient_u( ambientColor[0], ambientColor[1],
                                                                       ambientColor[2], ambientIntensity );

                                                  shader_update_ambient(&ambient_u);                                    }

//  - --- - --- - --- - --- -
