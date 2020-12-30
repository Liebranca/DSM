#include "SIN_Texture.h"

#include "lyarr/ZJC_Container.h"
#include "lybyte/ZJC_FileHandling.h"
#include "lyutils/ZJC_Evil.h"

#include "GL/glew.h"

#include <stdio.h>

#define  SIN_MAX_TEXTURES             128

static Texture  SIN_emptytex        = {0};

static ushort   SIN_ACTIVE_TEXTURES = 0;

static DAF*     TEX_ARCHIVE         = NULL;

static Container* SIN_texbucket     = NULL;

#define SIN_TEXFLAGS_SMOOTHMAP        1

//  - --- - --- - --- - --- -

int SIN_init_texbucket(uint top_id)             {

    SIN_texbucket = ZJC_build_cont              (SIN_MAX_TEXTURES, sizeof(Texture), top_id, "Texture"                   );
    return 0;                                                                                                           }

int SIN_end_texbucket()                         {

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    for(uint i = 0;
        i < SIN_MAX_TEXTURES; i++)              { Texture* tex = ((Texture*) SIN_texbucket->buff) + i;
                                                  if(tex->id) { glDeleteTextures(1, &tex->location); }                  }

    ZJC_del_cont(SIN_texbucket); return 0;                                                                              }

//  - --- - --- - --- - --- -

int SIN_fopen_texture (cchar* filename)         { return extraction_start(filename, 1, &TEX_ARCHIVE);                    }
int SIN_fclose_texture(cchar* filename)         { return extraction_end(filename, &TEX_ARCHIVE);                         }

//  - --- - --- - --- - --- -

Texture* SIN_findItem_texbucket(uint id,
                                int shutit)     {        ZJC_findItem_cont(SIN_texbucket, Texture, id, shutit );        }

Texture* SIN_getItem_texbucket (uint loc)       {        ZJC_getItem_cont (SIN_texbucket, Texture, loc        );        }
uint     SIN_findLoc_texbucket (uint id )       { return ZJC_findLoc_cont (SIN_texbucket, id,      0          ) - 1;    }

//  - --- - --- - --- - --- -

Texture* SIN_build_texture(uint       texid,
                           uint*      shdid,
                           uchar        offset,
                           SIN_MATDATA* matdata){

    Texture* tex = SIN_findItem_texbucket(texid, 1);

    if(tex == NULL)
    {
        if(SIN_ACTIVE_TEXTURES
           == SIN_MAX_TEXTURES)                 { fprintf(stderr, "Cannot create more than %u textures",
                                                  SIN_MAX_TEXTURES); return NULL;                                       }

        uint loc               = ZJC_push_cont  (SIN_texbucket, texid);
                                 WARD_EVIL_UNSIG(loc,           1    );

        Texture* tex           = ((Texture*) SIN_texbucket->buff) + loc;

        tex->id                = texid;
        tex->imcount           = 0;

        uint        size       = 0;

        float       fvalues[4] = { matdata->spec_mult, matdata->diff_mult,
                                   matdata->ref_mult,  matdata->glow_rea   };

        extractjoj(TEX_ARCHIVE,
                   offset,

                   &tex->imcount,
                   &size,
                   &tex->width,
                   &tex->height,

                   &matdata->flags,
                   fvalues,
                   shdid            );

        uint dim       = (tex->width) * (tex->height);
        float* pixels  = (float*) evil_malloc(dim * 3, sizeof(float));

        matdata->spec_mult = fvalues[0];
        matdata->diff_mult = fvalues[1];
        matdata->ref_mult  = fvalues[2];
        matdata->glow_rea  = fvalues[3];

//  - --- - --- - --- - --- -

        glGenTextures  (1, &tex->location                                                       );
        glActiveTexture(GL_TEXTURE0                                                             );
        glBindTexture  (GL_TEXTURE_2D_ARRAY, tex->location                                      );

        glTexStorage3D (GL_TEXTURE_2D_ARRAY, 4, GL_RGB8, tex->width, tex->height, tex->imcount  );

        for(uchar layer = 0; layer < tex->imcount; layer++)
        {
            joj_subTexRead(dim, tex->imcount, layer, &pixels);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0,  layer, tex->width, tex->height, 1,
                            GL_RGB,              GL_FLOAT, pixels                               );
        }

        glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0                          );
        glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL,  3                          );
        glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S,     GL_REPEAT                  );
        glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T,     GL_REPEAT                  );

        glTexParameterf (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST  );
        glTexParameterf (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST                 );

        WARD_EVIL_MFREE (pixels                                                                 );
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY                                                    );
        glBindTexture   (GL_TEXTURE_2D_ARRAY, 0                                                 );

        SIN_ACTIVE_TEXTURES++;

    }

    return tex;                                                                                                         }

//  - --- - --- - --- - --- -

void SIN_bind_texture(uint loc, uint slot)      {

    Texture* tex  = SIN_getItem_texbucket       (loc                                                                    );
    if(slot      >= 31)                         { slot = 31;                                                            }

    glActiveTexture                             (GL_TEXTURE0 + slot                                                     );
    glBindTexture                               (GL_TEXTURE_2D_ARRAY, tex->location                                     );
                                                                                                                        }

//  - --- - --- - --- - --- -

void SIN_del_texture(Texture* tex,
                     uint     loc)              {

    glBindTexture   (GL_TEXTURE_2D_ARRAY, 0);
    glDeleteTextures(1, &tex->location     );

    *tex = SIN_emptytex;

    SIN_ACTIVE_TEXTURES--;                                                                                              }

void  SIN_unsub_texture(uint loc)               {

    Texture* tex = SIN_getItem_texbucket(loc);

    if(tex)
    {
        tex->users--;
        if(tex->users == 0)                     { if( ZJC_pop_cont   (SIN_texbucket, tex->id)  )
                                                {     SIN_del_texture(tex,           loc    ); }                        }
    }

                                                                                                                        }

//  - --- - --- - --- - --- -
