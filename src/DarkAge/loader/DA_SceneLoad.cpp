#include "DA_SceneLoad.h"

#include "lybyte/ZJC_FileHandling.h"
#include "lyutils/ZJC_Evil.h"
#include "lyutils/ZJC_Str.h"

#include "../types/DA_GameObject.h"

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

        ushort    meshid =                      (resources[resid].mesh_idbase) + offset;

        glm::vec3 pos                           (ob->transform[0], ob->transform[1], ob->transform[2]                   );
        glm::quat rot                           (ob->transform[6], ob->transform[3], ob->transform[4], ob->transform[5] );
        glm::vec3 scale                         (ob->transform[7], ob->transform[8], ob->transform[9]                   );

        if(!resources[resid].loaded)            { DA_LOAD_RESOURCE(resources + resid, cwd);                             }
        DA_NODE*  node   = new DA_NODE          (meshid, pos, rot, scale                                                );

    }

    WARD_EVIL_MFREE(fullpath);

    }

//  - --- - --- - --- - --- -

