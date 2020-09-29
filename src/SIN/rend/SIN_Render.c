#include "SIN_Render.h"

#include "lyutils/ZJC_Evil.h"
#include "../types/SIN_Material.h"

#include <string.h>

//  - --- - --- - --- - --- -

#define SIN_MAX_RENDEROBJECTS   1024

int**   SIN_RenderBucket      = 0;
ushort  SIN_RenderBucket_size = 0;

//  - --- - --- - --- - --- -

void SIN_RenderBucket_init()                    {

    SIN_RenderBucket_size = SIN_getMaxMaterials(); 
    SIN_RenderBucket      = (int**) evil_malloc(SIN_RenderBucket_size + 1, sizeof(int*));

    for(uint i = 0; i < SIN_RenderBucket_size; i++)
    { SIN_RenderBucket[i] = (int*) evil_malloc(SIN_MAX_RENDEROBJECTS+1, sizeof(int));                                   }

    SIN_RenderBucket[SIN_RenderBucket_size] = (int*) evil_malloc(SIN_RenderBucket_size, sizeof(int));                   }

void SIN_RenderBucket_wipe()                    { for(uint i = 0; i < SIN_RenderBucket_size; i++)
                                                { memset((void*) SIN_RenderBucket[i], 0,
                                                         (SIN_MAX_RENDEROBJECTS + 1) * sizeof(int));                    }

                                                  memset((void*) SIN_RenderBucket[SIN_RenderBucket_size],
                                                             -1, SIN_RenderBucket_size * sizeof(int)                    );
                                                                                                                        }

void SIN_RenderBucket_end ()                    { for(ushort i = 0; i < SIN_RenderBucket_size+1; i++)
                                                { WARD_EVIL_MFREE(SIN_RenderBucket[i]);                                 }

                                                  WARD_EVIL_MFREE(SIN_RenderBucket);                                    }

//  - --- - --- - --- - --- -
