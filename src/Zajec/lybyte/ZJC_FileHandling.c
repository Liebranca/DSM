#include <stdio.h>
#include <stdint.h>

#include "ZJC_FileHandling.h"
#include "ZJC_BinTypes.h"

#include "../lyutils/ZJC_Evil.h"

//  - --- - --- - --- - --- -

#define EVIL_FREAD(type, count, buff)             buff = (type*) evil_malloc(count, sizeof(type));\
                                                  fread(buff, sizeof(type), count, curfile);

#define EVIL_FWRITE(type, count, buff, info)      WARD_EVIL_FWRITE(fwrite(buff, sizeof(type),\
                                                  count, curfile), info, count);

//  - --- - --- - --- - --- -


static    FILE*  curfile;
static    FILE*  tempfile;

const     ushort maxstride      =             0xFFFF;

cchar crk_sign[16]              =           { 0x46, 0x43, 0x4b, 0x42, 0x21, 0x54, 0x43, 0x48,
                                              0x45, 0x53, 0x47, 0x45, 0x54, 0x24, 0x24, 0x24                        };

//  - --- - --- - --- - --- -

typedef struct CrkFile                      {

    ushort vertCount;
    ushort indexCount;

    PhysVertexPacked3D* bounds;
    VertexPacked3D* verts;
    ushort* indices;

} CrkFile;

void del_CrkFile(CrkFile* crk)              { WARD_EVIL_MFREE(crk->bounds); WARD_EVIL_MFREE(crk->verts);
                                              WARD_EVIL_MFREE(crk->indices);                                        }

//  - --- - --- - --- - --- -

typedef struct IrfArchive                           {

    ushort fileCount;
    uint size;
    ushort offsets[512];

} IrfArchive;

ushort_pair archat (IrfArchive* irf, ushort i)  { ushort_pair pair = {0,0};
                                                  if( i <= 255 ) { pair.a = irf->offsets[i*2];
                                                                   pair.b = irf->offsets[(i*2)+1]; }
                                                  return pair;                                                          }

//  - --- - --- - --- - --- -

int openbin(cchar* filename,
            cchar* mode,
            int shutit)                    {

    int isnew = 0;
    curfile = fopen(filename, mode);

    if( (mode == "rb+")
        && (curfile == NULL) )                 { curfile = fopen(filename, "wb"); isnew = -1;                          }
                                                WARD_EVIL_FOPEN(curfile,  filename);

    if (!shutit)                            { printf("Opened file <%s>\n", filename);                               }

    return isnew;                                                                                                   }

//  - --- - --- - --- - --- -

int readarch(IrfArchive* irf,
                uint start,
                uint end)                       {

    ;
}

//  - --- - --- - --- - --- -

int closebin(cchar* filename,
             int shutit)                        {

    int success = fclose(curfile);
    WARD_EVIL_FCLOSE(success, filename);

    if (!shutit)                            { printf("File closed <%s>\n", filename);                               }

    return 0;                                                                                                       }

//  - --- - --- - --- - --- -

int IN_writecrk(cchar* filename,
                cchar* archive,
                ushort offset,
                ushort mode,
                CrkFile* crk,
                ushort* sizes,
                float* bounds,
                float* verts)               {

    int errorstate = 0;

    WARD_EVIL_WRAP(errorstate, openbin(filename, "rb", 0));

    fread(sizes, sizeof(ushort), 2, curfile);

    crk->vertCount = sizes[0];
    crk->indexCount = sizes[1];

    EVIL_FREAD(float, 24, bounds);
    EVIL_FREAD(float, crk->vertCount * 8, verts);
    EVIL_FREAD(ushort, crk->indexCount * 3, crk->indices);

    WARD_EVIL_WRAP(errorstate, closebin(filename, 0));

//  - --- - --- - --- - --- -

    crk->bounds = (PhysVertexPacked3D*) evil_malloc(8, sizeof(PhysVertexPacked3D));
    crk->verts = (VertexPacked3D*) evil_malloc(*sizes, sizeof(VertexPacked3D));

    for(ushort i = 0, j = 0;
        i < crk->vertCount * 8;
        i+= 8, j++ )                        { *(crk->verts+(j)) = build_vertpacked_3d(verts+i);                     }

    for(ushort i = 0, j = 0;
        i < 24; i+= 3, j++ )                { *(crk->bounds+(j)) = build_physvert_3d(bounds+i);                     }

    uint filesize = 4 + (8*6) + (crk->vertCount * 16) + (crk->indexCount * 2);

//  - --- - --- - --- - --- -

    {   int isnew = openbin(archive, "rb+", 0);
        WARD_EVIL_WRAP(errorstate, (isnew > -1) ? isnew : errorstate);

        ushort totalfiles = 1;
        ushort stride = 0;
        char sign[16];

        if(isnew)                           { EVIL_FWRITE(cchar, 16, &crk_sign, archive);
                                                EVIL_FWRITE(ushort, 1, &totalfiles, archive);                  }


        else 
        { 
            fread(sign,
                    sizeof(char),
                    16,
                    curfile);

            for(uint i = 0; i < 16; i++)    { if(sign[i] != crk_sign[i])
                                            { terminator(0x67, archive); return ERROR; }                      }
                                                  

            fread(&totalfiles,
                    sizeof(ushort),
                    1, curfile);

//  - --- - --- - --- - --- -

            if (offset < totalfiles) { ; }
            else if( offset == totalfiles ) { fseek(curfile, -2, SEEK_CUR);
                                                uint newtotalfiles = totalfiles + 1;
                                                fseek(curfile, 0, SEEK_CUR);
                                                EVIL_FWRITE(ushort, 1, &newtotalfiles, archive);

                                                fseek(curfile, 0, SEEK_CUR);
                                                totalfiles = newtotalfiles;                                           }

            for(uint i = 0;
                i < offset; i++)            { fread(&stride, sizeof(ushort), 1, curfile);
                                                fseek(curfile, stride, SEEK_CUR);

                                                if(stride > maxstride)
                                            { terminator(0x68, archive); return 0x02; }

                                                printf("Stride by %u at pos %u\n", stride, i);                        }
        }

    }

//  - --- - --- - --- - --- -

    EVIL_FWRITE(ushort, 2, sizes, archive);
    EVIL_FWRITE(PhysVertexPacked3D, 8, crk->bounds, archive);
    EVIL_FWRITE(VertexPacked3D, crk->vertCount, crk->verts, archive);
    EVIL_FWRITE(ushort, crk->indexCount, crk->indices, archive);
        
    WARD_EVIL_WRAP(errorstate, closebin(archive, 0));
    WARD_EVIL_WRAP(errorstate, remove(filename));
    printf("Deleted file <%s>\n", filename);

    return 0;
                                                                                                                    }

//  - --- - --- - --- - --- -

int writecrk(cchar* filename,
                cchar* archive,
                char* offset,
                char* mode)                 {

    CrkFile* crk = NULL;

    ushort sizes[2] = {0, 0};
    float* bounds = NULL;
    float* verts =  NULL;

    uint8_t numoffset = (uint8_t) hexstr_tolong(offset);
    uint8_t nummode   = (uint8_t) hexstr_tolong(mode);

    zjc_convertor_init();
    int evilstate = IN_writecrk(filename, archive, numoffset, nummode,
                                crk, sizes, bounds, verts);

    del_CrkFile(crk);
    zjc_convertor_end();

    WARD_EVIL_MFREE(bounds);
    WARD_EVIL_MFREE(verts);

    return evilstate;                                                                                               }

//  - --- - --- - --- - --- -
