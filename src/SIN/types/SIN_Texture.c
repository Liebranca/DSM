#include "SIN_Texture.h"

#include "lyarr/ZJC_Hash.h"
#include "lyarr/ZJC_Stack.h"
#include "lybyte/ZJC_FileHandling.h"
#include "lyutils/ZJC_Evil.h"

#include "GL\glew.h"

#include <stdio.h>

#define  SIN_MAX_TEXTURES             128

static Texture  SIN_emptytex        = {0};

static ushort   SIN_ACTIVE_TEXTURES = 0;

static DAF*     TEX_ARCHIVE         = {0};

static sStack*  SIN_TEX_SLOTSTACK   = NULL;
static sHash*   SIN_TEXHASH         = NULL;

static Texture  SIN_texbucket[SIN_MAX_TEXTURES];

//  - --- - --- - --- - --- -

int SIN_texbucket_init ()                       {

    SIN_TEXHASH       = build_sHash (SIN_MAX_TEXTURES);
    SIN_TEX_SLOTSTACK = build_sStack(SIN_MAX_TEXTURES);

    for(int i = SIN_MAX_TEXTURES-1;
        i > -1; i--)                            { sStack_push(SIN_TEX_SLOTSTACK, i);                                    }

    return 0;                                                                                                           }

int SIN_texbucket_end  ()                       {

    del_sStack(SIN_TEX_SLOTSTACK);
    del_sHash (SIN_TEXHASH);

    for(uint i = 0;
        i < SIN_MAX_TEXTURES; i++)              { Texture* tex = SIN_texbucket + i;

                                                  if(tex != NULL) 
                                                { glDeleteTextures(1, &tex->location); }                                }

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int     SIN_tex_extract_from(cchar* filename)  { return extraction_start(filename, 1, &TEX_ARCHIVE);                    }

int     SIN_tex_extract_end (cchar* filename)  { return extraction_end(filename, &TEX_ARCHIVE);                         }

//  - --- - --- - --- - --- -

Texture* SIN_texbucket_find (ushort id)         {

    ushort loc = sh_hashloc(SIN_TEXHASH, id);
    if(loc == 0)                                { return NULL;                                                          }

    return SIN_texbucket+(loc-1);                                                                                       }

ushort SIN_texbucket_findloc  (ushort id)       {

    ushort loc = sh_hashloc(SIN_TEXHASH, id);
    if(loc == 0)                                { fprintf(stderr, "Texture %u not found\n", id);
                                                  return 0;                                                             }

    return loc-1;                                                                                                       }

Texture* SIN_texbucket_get  (ushort loc)        {

    if(loc < SIN_MAX_TEXTURES)
    { 
        if(SIN_texbucket+loc)                   { return SIN_texbucket+loc;                                             }

        printf("Location %u points to an empty texture slot\n", loc);
        return NULL;
    }

    printf("No texture at location %u\n", loc);
    return NULL;                                                                                                        }

//  - --- - --- - --- - --- -

Texture* build_texture(ushort id, uchar offset) {

    Texture* tex = SIN_texbucket_find(id);

    if(tex == NULL)
    {
        if(SIN_ACTIVE_TEXTURES
           == SIN_MAX_TEXTURES)                 { fprintf(stderr, "Cannot create more than %u textures",
                                                  SIN_MAX_TEXTURES); return NULL;                                       }

        uint loc = sStack_pop(SIN_TEX_SLOTSTACK);
        WARD_EVIL_UNSIG(loc, 1);

        sh_insert(SIN_TEXHASH, id, loc);

        tex                 = SIN_texbucket+loc;

        tex->id             = id;
        uint*   size        = NULL;
        ushort* pixels      = NULL;

        extractjoj(TEX_ARCHIVE,
                   offset,
                   size,
                   &tex->width,
                   &tex->height,
                   pixels);

//  - --- - --- - --- - --- -

        uchar* pixel_data = (uchar*) evil_malloc(tex->width * tex->height, sizeof(uchar));
        uint   curpix     = 0;

        for(uint i = 0; i < *size; i++)
        {
            uchar  color = pixels[i] & 255, count = (pixels[i] & 65280) >> 8;
            for(uint j = 0; j < count; j++)     { pixel_data[curpix] = color; curpix++;                                 }

        }

        glGenTextures(1, &tex->location);
        glBindTexture(GL_TEXTURE_2D, tex->location);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->width, tex->height, 0,
                     GL_RGB, GL_UNSIGNED_BYTE_2_3_3_REV, pixel_data);

        glBindTexture(GL_TEXTURE_2D, 0);

        WARD_EVIL_MFREE(pixel_data);
        WARD_EVIL_MFREE(size);

        SIN_ACTIVE_TEXTURES++;

    }

    return tex;                                                                                                         }

//  - --- - --- - --- - --- -

void bind_tex_to_slot(ushort loc, uint slot)    {

    Texture* tex = SIN_texbucket_get(loc);

    if(slot <= 31)                              { slot = 31;                                                            }

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, tex->location);                                                                        }

//  - --- - --- - --- - --- -

void     del_tex            (Texture*   tex,
                             ushort loc)        {

    SIN_texbucket[loc] = SIN_emptytex;
    int memward = sStack_push(SIN_TEX_SLOTSTACK, loc);
    WARD_EVIL_UNSIG(memward, 1);

    SIN_ACTIVE_TEXTURES--;                                                                                              }

void  unsub_tex       (ushort loc)              {

    Texture* tex = SIN_texbucket_get(loc);

    if(tex)
    {
        tex->users--;
        if(tex->users == 0)                     { sh_pop(SIN_TEXHASH, tex->id); del_tex(tex, loc);                      }
    }

                                                                                                                        }

//  - --- - --- - --- - --- -
