#include "DA_ShaderLoad.h"

#include "SIN.h"

//  - --- - --- - --- - --- -

void DA_LOAD_SHADERS(const SHADERLOAD* shaders) {

    for(uint i = 0; i < shaders->num_shaders; i++)
    {
        ushort shdid  = shaders->idbase + i;
        Program* shd  = build_shader(shdid, shaders->v_Sources[i], shaders->p_Sources[i]  );

        ushort shdloc = SIN_shdbucket_findloc(shdid);

        shader_chkProgram (shdloc);
        SIN_UPDATE_AMBIENT();
    }
                                                                                                                        }

void DA_SHDENV_UPDATE(cushort* ids,
                      cushort num_shaders)      {

    for(ushort i = 0; i < num_shaders; i++)     { ushort shdloc = SIN_shdbucket_findloc(ids[i]);
                                                  shader_chkProgram(shdloc);

                                                  bind_tex_to_slot(SIN_texbucket_findloc(1), 0);
                                                  bind_tex_to_slot(SIN_texbucket_findloc(2), 1);                        }
                                                                                                                        }

