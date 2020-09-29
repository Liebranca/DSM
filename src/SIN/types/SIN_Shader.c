#include "SIN_Shader.h"

#include "lyarr/ZJC_Hash.h"
#include "lyarr/ZJC_Stack.h"
#include "lyutils/ZJC_Evil.h"

#include "../SIN_Constants.h"

#include "GL/glew.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>

static Program  SIN_emptyshd       = {0};
static ushort   SIN_ACTIVE_SHADERS = 0;

static sStack*  SIN_SHD_SLOTSTACK  = NULL;
static sHash*   SIN_SHDHASH        = NULL;

static Program* SIN_shdbucket      = NULL;

//  - --- - --- - --- - --- -

Program*        __program          = NULL;
ushort          __curShdLoc        = -1;

void shader_reset_loc  ()                       { __program = NULL; __curShdLoc = -1;                                   }
void shader_setProgram (ushort loc)             { __program = SIN_shdbucket_get(loc); __curShdLoc = loc+1;              }
void shader_useProgram ()                       { glUseProgram(__program->location);                                    }

int  shader_chkProgram (ushort loc)             {
    if (loc+1 != __curShdLoc)                   { shader_setProgram(loc); shader_useProgram(); return 1; } return 0;    }

//  - --- - --- - --- - --- -

int       SIN_shdbucket_init  ()                {

    SIN_shdbucket     = (Program*) evil_malloc(SIN_MAX_SHADERS, sizeof(Program));

    SIN_SHDHASH       = build_sHash (SIN_MAX_SHADERS);
    SIN_SHD_SLOTSTACK = build_sStack(SIN_MAX_SHADERS);

    for(int i = SIN_MAX_SHADERS-1; i > 0; i--)  { sStack_push(SIN_SHD_SLOTSTACK, i);                                    }

    return 0;                                                                                                           }

void shader_free(Program* program)              {

    for (uint i = 0; i < 2; i++)                { glDetachShader(program->location, program->shaders[i]);
                                                  glDeleteShader(program->shaders[i]);                                  }

    glDeleteProgram(program->location);                                                                                 };

int       SIN_shdbucket_end   ()                {

    del_sStack(SIN_SHD_SLOTSTACK);
    del_sHash (SIN_SHDHASH);

    for(uint i = 0;
        i < SIN_MAX_SHADERS; i++)               { Program* program = SIN_shdbucket + i;

                                                  if(program->id)
                                                { shader_free(program); }                                               }

    WARD_EVIL_MFREE(SIN_shdbucket);

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

Program* SIN_shdbucket_find  (ushort id)        {

    ushort loc = sh_hashloc(SIN_SHDHASH, id);
    if(loc == 0)                                { return NULL;                                                          }

    return SIN_shdbucket+(loc-1);                                                                                       }

ushort SIN_shdbucket_findloc  (ushort id)       {

    ushort loc = sh_hashloc(SIN_SHDHASH, id);
    if(loc == 0)                                { fprintf(stderr, "Shader program %u not found\n", id);
                                                  return 0;                                                             }

    return loc-1;                                                                                                       }

Program* SIN_shdbucket_get  (ushort loc)        {

    if(loc < SIN_MAX_SHADERS)
    { 
        if(SIN_shdbucket+loc)                   { return SIN_shdbucket+loc;                                             }

        fprintf(stderr, "Location %u points to an empty shader program slot\n", loc);
        return NULL;
    }

    fprintf(stderr, "No shader program at location %u\n", loc);
    return NULL;                                                                                                        }

//  - --- - --- - --- - --- -

void checkShaderError(uint shader,
                             uint flag,
                             int isProgram,
                             const char* errorMessage)
{
    int success = 0;
    char error[1024] = { 0 };

    if (isProgram)                              { glGetProgramiv(shader, flag, &success);                               }
    else                                        { glGetShaderiv(shader, flag, &success);                                }

    if (success == GL_FALSE)
    {
        if (isProgram)                          { glGetProgramInfoLog(shader, sizeof(error), NULL, error);              }
        else                                    { glGetShaderInfoLog(shader, sizeof(error), NULL, error);               }

        fprintf(stderr, "%s: %s\n", errorMessage, error);
    }
}

uint createShader(cchar** source, uint shaderType)
{
    uint shader = glCreateShader(shaderType);
    if (!shader)                                { fprintf(stderr, "Shader couldn't be created\n");                      }

    glShaderSource(shader, 1, source, NULL);
    glCompileShader(shader);

    checkShaderError(shader, GL_COMPILE_STATUS, 0, "Shader couldn't compile");

    return shader;                                                                                                      }

//  - --- - --- - --- - --- -

Program* build_shader(ushort id,
                     cchar** vert_source,
                     cchar** frag_source)       {

    Program* program = SIN_shdbucket_find(id);

    if(program == NULL)
    {
        if(SIN_ACTIVE_SHADERS
        == SIN_MAX_SHADERS)                     { fprintf(stderr, "Cannot create more than %u shader programs\n",
                                                  SIN_MAX_SHADERS); return NULL;                                        }

        uint loc = sStack_pop(SIN_SHD_SLOTSTACK);
        WARD_EVIL_UNSIG(loc, 1);

        sh_insert(SIN_SHDHASH, id, loc);

        program             = SIN_shdbucket+loc;

        program->location   = glCreateProgram();

        program->shaders[0] = createShader(vert_source, GL_VERTEX_SHADER);
        program->shaders[1] = createShader(frag_source, GL_FRAGMENT_SHADER);

        for (uint i = 0; i < 2; i++)            { glAttachShader(program->location, program->shaders[i]);               }

        glBindAttribLocation(program->location, 0, "Position");
        glBindAttribLocation(program->location, 1, "Normal");
        glBindAttribLocation(program->location, 2, "Tangent");
        glBindAttribLocation(program->location, 3, "Bitangent");
        glBindAttribLocation(program->location, 4, "UV");

        glLinkProgram    (program->location);
        checkShaderError (program->location, GL_LINK_STATUS, 1, "Shader linking failed");
        glValidateProgram(program->location);
        checkShaderError (program->location, GL_VALIDATE_STATUS, 1, "Shader validation failed");

        program->uniforms[SIN_TRANSFORM_U]  = glGetUniformLocation(program->location, "Model");
        program->uniforms[SIN_NORMAL_U]     = glGetUniformLocation(program->location, "ModelInverseTranspose");
        program->uniforms[SIN_PROJECTION_U] = glGetUniformLocation(program->location, "ViewProjection");
        program->uniforms[SIN_CAMFWD_U]     = glGetUniformLocation(program->location, "CamFwd");
        program->uniforms[SIN_CAMPOS_U]     = glGetUniformLocation(program->location, "CamPos");
        program->uniforms[SIN_AMBIENT_U]    = glGetUniformLocation(program->location, "Ambient");
        program->uniforms[SIN_NUMLIGHTS_U]  = glGetUniformLocation(program->location, "NUM_LIGTS");
        program->uniforms[SIN_LIGHTS_U]     = glGetUniformLocation(program->location, "Lights");

        glUseProgram(program->location);

        int env1Loc    = glGetUniformLocation(program->location, "shineRough" );
        int env2Loc    = glGetUniformLocation(program->location, "shineSoft"  );
        int depthLoc   = glGetUniformLocation(program->location, "DepthMap"   );

        int diffuseLoc = glGetUniformLocation(program->location, "DiffuseMap" );
        int infoLoc    = glGetUniformLocation(program->location, "ShadingInfo");
        int normalLoc  = glGetUniformLocation(program->location, "NormalMap"  );

        glUniform1i(env1Loc,    0                 );
        glUniform1i(env2Loc,    1                 );

        glUniform1i(diffuseLoc, SIN_TEXID_BASE + 0);
        glUniform1i(infoLoc,    SIN_TEXID_BASE + 1);
        glUniform1i(normalLoc,  SIN_TEXID_BASE + 2);

        glUniform1i(depthLoc,   SIN_TEXID_BASE + 4);

        SIN_ACTIVE_SHADERS++;

    }

    return program;                                                                                                     };

//  - --- - --- - --- - --- -

void    del_shader        (Program* program,
                           ushort loc)          {

    shader_free(program);

    SIN_shdbucket[loc] = SIN_emptyshd;
    int memward = sStack_push(SIN_SHD_SLOTSTACK, loc);
    WARD_EVIL_UNSIG(memward, 1);

    SIN_ACTIVE_SHADERS--;                                                                                               }

void     unsub_shader       (ushort loc)        {

    Program* program = SIN_shdbucket+loc;
    if(program)
    {
        program->users--;
        if(program->users == 0)                 { sh_pop(SIN_SHDHASH, loc); del_shader(program, loc);                   }
    }
                                                                                                                        }
