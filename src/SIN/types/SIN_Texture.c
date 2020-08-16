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

static DAF*     TEX_ARCHIVE         = NULL;

static sStack*  SIN_TEX_SLOTSTACK   = NULL;
static sHash*   SIN_TEXHASH         = NULL;

static Texture* SIN_texbucket       = NULL;

//  - --- - --- - --- - --- -

int SIN_texbucket_init ()                       {

    TEX_ARCHIVE       = dafalloc();
    SIN_texbucket     = (Texture*) evil_malloc(SIN_MAX_TEXTURES, sizeof(Texture));

    SIN_TEXHASH       = build_sHash (SIN_MAX_TEXTURES);
    SIN_TEX_SLOTSTACK = build_sStack(SIN_MAX_TEXTURES);

    for(int i = SIN_MAX_TEXTURES-1;
        i > 0; i--)                             { sStack_push(SIN_TEX_SLOTSTACK, i);                                    }

    return 0;                                                                                                           }

int SIN_texbucket_end  ()                       {

    del_sStack(SIN_TEX_SLOTSTACK);
    del_sHash (SIN_TEXHASH);

    for(uint i = 0;
        i < SIN_MAX_TEXTURES; i++)              { Texture* tex = SIN_texbucket + i;

                                                  if(tex->id)
                                                { glDeleteTextures(1, &tex->location); }                                }

    WARD_EVIL_MFREE(SIN_texbucket);
    WARD_EVIL_MFREE(TEX_ARCHIVE);

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

void loadtex(float*  pixel_data,
             uint size,
             ushort* pixels,
             int     isNormalMap)               {

    uint   curpix     = 0;

    for(uint i = 0; i < size; i++)
    {

        ushort color_index =  pixels[i] & (0b0001111111111111);
        uchar  count       = (pixels[i] & (0b1110000000000000)) >> 13;
        count             += 1;

        float  r = ( color_index &  31              ) * 0.03125f;
        float  g = ((color_index & (31 << 5 )) >> 5 ) * 0.03125f;
        float  b = ((color_index & (7  << 10)) >> 10) * 0.125f;

        if (isNormalMap)                    { b = 1.0f;                                                                 }

        for(uint j = 0; j < count; j++)     { pixel_data[curpix+0] = r;
                                                pixel_data[curpix+1] = g;
                                                pixel_data[curpix+2] = b;

                                                curpix += 3;                                                            }

    }
                                                                                                                        }

//  - --- - --- - --- - --- -

Texture* build_texture(ushort id,
                       uchar offset,
                       int flags)               {

    Texture* tex = SIN_texbucket_find(id);

    if(tex == NULL)
    {
        if(SIN_ACTIVE_TEXTURES
           == SIN_MAX_TEXTURES)                 { fprintf(stderr, "Cannot create more than %u textures",
                                                  SIN_MAX_TEXTURES); return NULL;                                       }

        int isNormalMap = flags & SIN_TEXFLAGS_NORMALMAP;
        int isCubeMap   = flags & SIN_TEXFLAGS_CUBEMAP;

        uint loc = sStack_pop(SIN_TEX_SLOTSTACK);
        WARD_EVIL_UNSIG(loc, 1);

        sh_insert(SIN_TEXHASH, id, loc);

        tex                 = SIN_texbucket+loc;

        tex->id           = id;
        uint  size        = 0;
        ushort* pixels    = NULL;

        extractjoj(TEX_ARCHIVE,
                   offset,
                   &size,
                   &tex->width,
                   &tex->height,
                   &pixels);

//  - --- - --- - --- - --- -

        float* pixel_data = (float*) evil_malloc(tex->width * tex->height * 3, sizeof(float));
        loadtex(pixel_data, size, pixels, isNormalMap);

        glGenTextures(1, &tex->location);
        glBindTexture(GL_TEXTURE_2D, tex->location);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->width, tex->height, 0,
                     GL_RGB, GL_FLOAT, pixel_data);

        WARD_EVIL_MFREE(pixel_data);

        SIN_ACTIVE_TEXTURES++;

    }

    return tex;                                                                                                         }

//  - --- - --- - --- - --- -

void bind_tex_to_slot(ushort loc, uint slot)    {

    Texture* tex = SIN_texbucket_get(loc);

    if(slot >= 31)                              { slot = 31;                                                            }

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, tex->location);                                                                        }

//  - --- - --- - --- - --- -

void     del_tex            (Texture*   tex,
                             ushort loc)        {

    glDeleteTextures(1, &tex->location);

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
