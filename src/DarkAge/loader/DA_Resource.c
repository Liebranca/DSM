#include "DA_Resource.h"

#include "lyutils/ZJC_Evil.h"
#include "lyutils/ZJC_Str.h"

#include "types/SIN_Texture.h"
#include "types/SIN_Material.h"
#include "types/SIN_Mesh.h"
#include "types/SIN_MeshBatch.h"

#include <stdio.h>

//  - --- - --- - --- - --- -

void DA_LOAD_RESOURCE(RESOURCE* res,
                      cchar*    cwd  )          {

    if(res->num_textures)
    {
        cchar* tex_archive = catpath(cwd, res->tex_archive);

        SIN_tex_extract_from(tex_archive);
        for(uint i = 0;
            i < res->num_textures; i++)         { SIN_buildTexture (res->tex_idbase + i,
                                                                    res->tex_offset + i  );                             }

        SIN_tex_extract_end(tex_archive);
        WARD_EVIL_MFREE    (tex_archive);
    }

//  - --- - --- - --- - --- -

    if(res->num_materials)
    {
        uint j = 0;
        for(uint i = 0;
            i < res->num_materials; i++)        { SIN_buildMaterial(res->mat_idbase         + i,
                                                                    res->mat_opaque          [i],
                                                                    res->mat_texture_idbase + i,
                                                                    res->mat_shader_ids      [i]  );                    }
    }

//  - --- - --- - --- - --- -

    if(res->num_meshes)
    {
        SIN_genMeshBatch();
        cchar* mesh_archive = catpath(cwd, res->mesh_archive);

        SIN_mesh_extract_from(mesh_archive);
        for(uint i = 0;
            i < res->num_meshes; i++)           { SIN_buildMesh( res->mesh_idbase + i, res->mesh_offset + i  );         }

        SIN_mesh_extract_end(mesh_archive);
        WARD_EVIL_MFREE     (mesh_archive);
    }

    res->loaded = 1;                                                                                                    }

//  - --- - --- - --- - --- -

