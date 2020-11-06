#include "DA_ShaderLoad.h"

#include "SIN.h"

//  - --- - --- - --- - --- -

void DA_LOAD_SHADERS(const SHADERLOAD* shaders) {

    for(uint i = 0; i < shaders->num_shaders; i++)
    {
        ushort shdid  = shaders->idbase + i;
        Program* shd  = SIN_buildShader(shdid, shaders->params[i]);

        ushort shdloc = SIN_shdbucket_findloc(shdid);

        SIN_chkProgram    (shdloc);
        SIN_UPDATE_AMBIENT(      );
    }
                                                                                                                        }

void DA_SHDENV_UPDATE(cushort* ids,
                      cushort num_shaders)      {

    for(ushort i = 0; i < num_shaders; i++)     { ushort shdloc = SIN_shdbucket_findloc(ids[i]);
                                                  SIN_chkProgram                       (shdloc);

                                                  SIN_bindTexture(SIN_texbucket_findloc(2), 0);                         }
                                                                                                                        }

