#include "DA_SceneLoad.h"

#include "lybyte/ZJC_FileHandling.h"
#include "lyutils/ZJC_Evil.h"
#include "lyutils/ZJC_Str.h"

#include "../types/DA_Lamp.h"

//  - --- - --- - --- - --- -

void DA_LOAD_SCENE(RESOURCE* resources,
                   cchar*    filename,
                   cchar*    cwd)               {

    SSX ssx         = { 0 };
    cchar* fullpath = catpath(cwd, filename);

    readssx(&ssx, fullpath);

    for(ushort i = 0; i < ssx.count; i++)
    {

        SSO*      ob     =                      ssx.objects + i;

        ushort    resid  =                      ob->resinfo[0];
        ushort    offset =                      ob->resinfo[1];
        ushort    flags  =                      ob->resinfo[2];

        ushort    meshid =                      (resources[resid].mesh_idbase) + offset;

        glm::vec3 pos                           (ob->fvalues[0], ob->fvalues[1], ob->fvalues[2]                         );
        glm::quat rot                           (ob->fvalues[3], ob->fvalues[4], ob->fvalues[5], ob->fvalues[6]         );
        glm::vec3 scale                         (ob->fvalues[7], ob->fvalues[8], ob->fvalues[9]                         );

        if(!resources[resid].loaded)            { DA_LOAD_RESOURCE(resources + resid, cwd);                             }

        if(flags & DA_NF_LIGHTSOURCE) 
        {
            glm::vec4 lpos                      (ob->fvalues[10], ob->fvalues[11], ob->fvalues[12], 1                   );
            glm::vec4 color                     (ob->fvalues[13], ob->fvalues[14], ob->fvalues[15], ob->fvalues[16]     );
            glm::vec3 ldirn                     (ob->fvalues[17], ob->fvalues[18], ob->fvalues[19]                      );

            float     rad  =                    ob->fvalues[20];
            float     atte =                    ob->fvalues[21];

            DA_LAMP*  lamp =                    new DA_LAMP(meshid, flags, pos,   rot, scale,
                                                            lpos,   color, ldirn, rad, atte                             );
        }

        else
        {   DA_NODE*  node =                    new DA_NODE(meshid, flags, pos, rot, scale);                            }

    }

    del_SsxFile(&ssx);
    WARD_EVIL_MFREE(fullpath);

    }

//  - --- - --- - --- - --- -

