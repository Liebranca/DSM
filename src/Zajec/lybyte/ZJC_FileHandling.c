#include <stdio.h>
#include <stdint.h>

#include "ZJC_FileHandling.h"
#include "ZJC_BinTypes.h"

#include "../lyutils/ZJC_Evil.h"

//  - --- - --- - --- - --- -

#define EVIL_FREAD(type, count, buff)            buff = (type*) evil_malloc(count, sizeof(type));\
                                                 fread(buff, sizeof(type), count, curfile);

#define EVIL_FWRITE(type, count, buff, info)     WARD_EVIL_FWRITE(fwrite(buff, sizeof(type),\
                                                 count, curfile), info, count);

//  - --- - --- - --- - --- -

FILE*  curfile;

cushort         IRF_HSIZE       =                 0x40e;

const uint8_t   IRF_WRITE       =                 0x00;
const uint8_t   IRF_APPEND      =                 0x01;
const uint8_t   IRF_UPDATE      =                 0x02;
const uint8_t   IRF_READ        =                 0x03;
const uint8_t   IRF_DELETE      =                 0x04;

cushort MAXSTRIDE               =                 0xFFFF;

cchar CRKSIGN[8]                =               { 0x4c, 0x59, 0x45, 0x42, 0x24, 0x43, 0x52, 0x4b                        };

cchar FBGM[16]                  =               { 0x46, 0x43, 0x4b, 0x42, 0x21, 0x54, 0x43, 0x48,
                                                  0x45, 0x53, 0x47, 0x45, 0x54, 0x24, 0x24, 0x24                        };

//  - --- - --- - --- - --- -

cchar* get_fmode(uint8_t mode)                  {

    if     (mode == IRF_WRITE )                 { return "wb+";                                                         }
    else if(mode == IRF_READ )                  { return "rb";                                                          }
    else if(mode == IRF_APPEND )                { return "ab+";                                                         }
    else                                        { return "rb+";                                                         }
                                                                                                                        }

int openbin(cchar* filename,
            cchar* mode,
            int    shutit)                      {

    int isnew = 0;
    curfile = fopen(filename, mode);

    int readmode = (mode == "rb+") || (mode == "rb");

    if( readmode && (curfile == NULL) )         { curfile = fopen(filename, "wb"); isnew = -1;                          }

    WARD_EVIL_FOPEN(curfile,  filename);

    if (!shutit)                                { printf("Opened file <%s>\n", filename);                               }
    return isnew;                                                                                                       }

//  - --- - --- - --- - --- -

int closebin(cchar* filename,
             int    shutit)                     {

    int success = fclose(curfile);
    WARD_EVIL_FCLOSE(success, filename);

    if (!shutit)                                { printf("File closed <%s>\n", filename);                               }
    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

typedef struct CrkFile                          {

    ushort  size;
    ushort  vertCount;
    ushort  indexCount;

    pVP3D*  bounds;
    VP3D*   verts;
    ushort* indices;

} CrkFile;

void del_CrkFile(CrkFile* crk)                  {

    WARD_EVIL_MFREE(crk->bounds);
    WARD_EVIL_MFREE(crk->verts);
    WARD_EVIL_MFREE(crk->indices);                                                                                      }

//  - --- - --- - --- - --- -

typedef struct IrfArchive                       {

    ushort   fileCount;
    uint32_t size;
    uint32_t offsets[256];
    uchar*   data;

} IrfArchive;

int openarch(cchar*        filename,
             cchar*        readmode,
             cchar         archtype[],
             const uint8_t mode,
             IrfArchive*   irf,
             int           shutit)              {

    int isnew = 0, errorstate = 0;
    irf->fileCount = 0;
    irf->size = 8 + 2 + 4 + 1024;

    isnew = openbin(filename, readmode, shutit);
    WARD_EVIL_WRAP(errorstate, (isnew > -1) ? isnew : errorstate);

    if(isnew || mode == IRF_WRITE)              { for(uint i = 1; i < 256; i++) { irf->offsets[i] = 0; }

                                                  EVIL_FWRITE(cchar,    8,   archtype,        filename);
                                                  EVIL_FWRITE(ushort,   1,   &irf->fileCount, filename);
                                                  EVIL_FWRITE(uint32_t, 1,   &irf->size,      filename);
                                                  EVIL_FWRITE(uint32_t, 256, irf->offsets,    filename);
                                                  fseek(curfile, 0, SEEK_CUR);                                          }

//  - --- - --- - --- - --- -

    else
    {

        if(mode == IRF_APPEND)                  { rewind(curfile);                                                      }

        char sign[8];
        fread(sign, sizeof(char), 8, curfile);

        for(uint i = 0; i < 8; i++)             { if(sign[i] != archtype[i])
                                                { terminator(0x67, filename); return ERROR; }                           }

        fread(&irf->fileCount, sizeof(ushort), 1, curfile);

        fread(&irf->size,   sizeof(uint32_t), 1,   curfile);
        fread(irf->offsets, sizeof(uint32_t), 512, curfile);

        uint32_t datasize = irf->size - IRF_HSIZE;

        if((mode == IRF_UPDATE)
        || (mode == IRF_DELETE))                { EVIL_FREAD(uchar, datasize, irf->data);                               }

    }

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int readarch(IrfArchive* irf,
             uint        start,
             uint        end)                   {

    ;
}

//  - --- - --- - --- - --- -

int read_crkdump(cchar*     filename,
                 CrkFile*    crk,
                 float*      bounds,
                 float*      verts)             {

    int errorstate = 0;

    WARD_EVIL_WRAP(errorstate, openbin(filename, "rb", 0));

    ushort sizes[2] = {0, 0};
    fread(sizes, sizeof(ushort), 2, curfile);

    crk->vertCount = sizes[0];
    crk->indexCount = sizes[1];

    EVIL_FREAD(float,  24,                  bounds);
    EVIL_FREAD(float,  crk->vertCount * 8,  verts);
    EVIL_FREAD(ushort, crk->indexCount * 3, crk->indices);

    WARD_EVIL_WRAP(errorstate, closebin(filename, 0));

//  - --- - --- - --- - --- -

    crk->size   = 4 + (8*6) + (crk->vertCount * 16) + (crk->indexCount * 2);
    crk->bounds = (pVP3D*) evil_malloc(8,      sizeof(pVP3D));
    crk->verts  = (VP3D*)  evil_malloc(*sizes, sizeof(VP3D) );

    for(uint i = 0, j = 0;
        i < (uint) crk->vertCount * 8;
        i+= 8, j++ )                            { *(crk->verts+j) =  build_vertpacked_3d(verts+i);                      }

    for(uint i = 0, j = 0;
        i < 24; i+= 3, j++ )                    { *(crk->bounds+j) = build_physvert_3d(bounds+i);                       }

    WARD_EVIL_WRAP(errorstate, remove(filename));
    printf("Deleted file <%s>\n", filename);

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int crk_to_irf(CrkFile*    crk,
               cchar* filename)                 {

    ushort sizes[2] = { crk->vertCount, crk->indexCount};

    fseek(curfile, 0, SEEK_CUR);
    EVIL_FWRITE(ushort, 2,               sizes,        filename);
    EVIL_FWRITE(pVP3D,  8,               crk->bounds,  filename);
    EVIL_FWRITE(VP3D,   crk->vertCount,  crk->verts,   filename);
    EVIL_FWRITE(ushort, crk->indexCount, crk->indices, filename);
    fseek(curfile, 0, SEEK_CUR);

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int writeirf(CrkFile* crk,
             IrfArchive* irf,
             uint8_t mode,
             uint8_t offset,
             cchar* filename)                   {

    int evilstate = 0;

    {
        cchar* readmode = get_fmode(mode);
        WARD_EVIL_WRAP(evilstate, openarch(filename,
                                           readmode,
                                           CRKSIGN,
                                           mode,
                                           irf,
                                           0        ));
    }

    if      (mode == IRF_WRITE)
    {
        WARD_EVIL_WRAP(evilstate, crk_to_irf(crk, filename));
        irf->fileCount++;

        rewind(curfile);
        fseek (curfile, 8, SEEK_CUR);
        fseek (curfile, 0, SEEK_CUR);

        uint32_t newsize_offset[2] = { irf->size + crk->size, IRF_HSIZE };

        EVIL_FWRITE(ushort,   1, &irf->fileCount, filename);
        EVIL_FWRITE(uint32_t, 2, newsize_offset,  filename);

    }

    else if (mode == IRF_APPEND)
    {
        WARD_EVIL_WRAP(evilstate, crk_to_irf(crk, filename));

        closebin(filename, 1);
        openbin (filename, "rb+", 1);

        fseek (curfile, 8, SEEK_CUR);
        fseek (curfile, 0, SEEK_CUR);

        uint32_t newsize = irf->size + crk->size;
        uint32_t newoffset = irf->size;
        uint16_t offset_stride = (irf->fileCount) * 4;

        irf->fileCount++;
        EVIL_FWRITE(ushort,   1, &irf->fileCount,  filename);
        EVIL_FWRITE(uint32_t, 1, &newsize,         filename);

        fseek (curfile, 0,             SEEK_CUR);
        fseek (curfile, offset_stride, SEEK_CUR);

        EVIL_FWRITE(uint32_t, 1, &newoffset,       filename);
    }

    closebin(filename, 0);
    return 0;
                                                                                                                        }

//  - --- - --- - --- - --- -

int writecrk(cchar* filename,
             cchar* archive,
             char*  mode,
             char*  offset)                     {

    int evilstate =    0;

    IrfArchive irf =  {0};
    CrkFile crk =     {0};

    float* bounds =    NULL;
    float* verts  =    NULL;

    uint8_t numoffset = (uint8_t) hexstr_tolong(offset);
    uint8_t nummode   = (uint8_t) hexstr_tolong(mode);

    zjc_convertor_init();
    evilstate = read_crkdump(filename, &crk, bounds, verts);

    zjc_convertor_end();

    WARD_EVIL_MFREE(bounds);
    WARD_EVIL_MFREE(verts);

    if(!evilstate)                              { evilstate = writeirf(&crk, &irf, nummode, numoffset, archive);        }
    if(nummode == IRF_UPDATE || IRF_DELETE)     { WARD_EVIL_MFREE(irf.data);                                            }

    del_CrkFile(&crk);

    return evilstate;                                                                                                   }

//  - --- - --- - --- - --- -
