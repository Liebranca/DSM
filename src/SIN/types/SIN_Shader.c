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

void SIN_resetShdloc()                          { __program = NULL; __curShdLoc = -1;                                   }
void SIN_setProgram (ushort loc)                { __program = SIN_shdbucket_get(loc); __curShdLoc = loc+1;              }
void SIN_useProgram ()                          { glUseProgram(__program->location);                                    }

int  SIN_chkProgram (ushort loc)                {
    if (loc+1 != __curShdLoc)                   { SIN_setProgram(loc); SIN_useProgram(); return 1; } return 0;          }

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

void SIN_checkShaderError(uint shader,
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

uint SIN_createShader(cchar** source, uchar num_sources, uint shaderType)
{
    uint shader = glCreateShader(shaderType);
    if (!shader)                                { fprintf(stderr, "Shader couldn't be created\n");                      }

    glShaderSource      (shader, num_sources, source, NULL                       );
    glCompileShader     (shader                                                  );

    SIN_checkShaderError(shader, GL_COMPILE_STATUS, 0, "Shader couldn't compile" );

    return shader;                                                                                                      }

//  - --- - --- - --- - --- -

Program* SIN_buildShader (ushort id, const shaderParams* shader)
{

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
        program->flags      = shader->flags;

//  - --- - --- - --- - --- -

        program->shaders[0] = SIN_createShader(shader->source_v, shader->num_vsources, GL_VERTEX_SHADER  );
        program->shaders[1] = SIN_createShader(shader->source_f, shader->num_fsources, GL_FRAGMENT_SHADER);

        for (uint i = 0; i < 2; i++)            { glAttachShader(program->location, program->shaders[i]);               }

        for(uchar attrib_loc = 0; attrib_loc < shader->num_attribs; attrib_loc++)
        { glBindAttribLocation(program->location, attrib_loc, shader->attribs[attrib_loc]); }

        glLinkProgram       (program->location                                                   );
        SIN_checkShaderError(program->location, GL_LINK_STATUS, 1, "Shader linking failed"       );

        glUseProgram        (program->location                                                   );

//  - --- - --- - --- - --- -

        for(uchar i = 0; i < shader->num_uniforms; i++)
        {
            int uniform_loc = -1;

            if     (!strcmp(shader->uniforms[i], "Model"                )) { uniform_loc = SIN_MODEL_U; }
            else if(!strcmp(shader->uniforms[i], "ModelInverseTranspose")) { uniform_loc = SIN_NMAT_U;  }

            if(uniform_loc >= 0)
            { program->uniforms[uniform_loc] = glGetUniformLocation(program->location, shader->uniforms[i]); }
        }

        for(uchar i = 0; i < shader->num_ubos; i++)
        {
            int block_binding = -1;

            if     (!strcmp(shader->ubos[i], "Ambient")) { block_binding = 0; }
            else if(!strcmp(shader->ubos[i], "Lights" )) { block_binding = 1; }

            if(block_binding >= 0)
            {
                uint block_index = glGetUniformBlockIndex(program->location, shader->ubos[i]           );
                glUniformBlockBinding                    (program->location, block_index, block_binding);
            }
        }

//  - --- - --- - --- - --- -

        for(uchar i = 0; i < shader-> num_samplers; i++)
        {
            int sampler_loc = glGetUniformLocation(program->location, shader->samplers[i]);
            int texslot     = SIN_TEXID_BASE;

            if     (!strcmp(shader->samplers[i], "ShadowMap")) { texslot = 0;       }
            else if(!strcmp(shader->samplers[i], "ENV"      )) { texslot = 1;       }
            else if(!strcmp(shader->samplers[i], "Surface"  )) { texslot = 2;       }

            glUniform1i                                        (sampler_loc, texslot);
        }

        glValidateProgram   (program->location                                                   );
        SIN_checkShaderError(program->location, GL_VALIDATE_STATUS, 1, "Shader validation failed");

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

void     SIN_unsubShader       (ushort loc)     {

    Program* program = SIN_shdbucket+loc;
    if(program)
    {
        program->users--;
        if(program->users == 0)                 { sh_pop(SIN_SHDHASH, loc); del_shader(program, loc);                   }
    }
                                                                                                                        }
