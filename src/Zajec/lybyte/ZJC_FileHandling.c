#include <stdio.h>
#include <stdint.h>
#include <io.h>

#include "ZJC_FileHandling.h"

#include "../lymath/ZJC_GOPS.h"
#include "../lyutils/ZJC_Evil.h"

//  - --- - --- - --- - --- -

#define EVIL_FREAD(type, count, buff)            buff = (type*) evil_malloc(count, sizeof(type));\
                                                 fread(buff, sizeof(type), count, curfile);

#define EVIL_FWRITE(type, count, buff, info)     WARD_EVIL_FWRITE(fwrite(buff, sizeof(type),\
                                                 count, curfile), info, count);

//  - --- - --- - --- - --- -

FILE*           curfile;

cushort         DAF_HSIZE       =                 0x40e;

const uint8_t   DAF_WRITE       =                 0x00;
const uint8_t   DAF_APPEND      =                 0x01;
const uint8_t   DAF_UPDATE      =                 0x02;
const uint8_t   DAF_READ        =                 0x03;
const uint8_t   DAF_DELETE      =                 0x04;

cushort         MAXSTRIDE       =                 0xFFFF;

cchar           CRKSIGN[8]      =               { 0x4c, 0x59, 0x45, 0x42, 0x24, 0x43, 0x52, 0x4b                        };
cchar           JOJSIGN[8]      =               { 0x4c, 0x59, 0x45, 0x42, 0x24, 0x4a, 0x4f, 0x4a                        };

cchar           FBGM[16]        =               { 0x46, 0x43, 0x4b, 0x42, 0x21, 0x54, 0x43, 0x48,
                                                  0x45, 0x53, 0x47, 0x45, 0x54, 0x24, 0x24, 0x24                        };

const uint8_t   READ_MESH       =                 0x00;
const uint8_t   READ_TEX        =                 0x01;
const uint8_t   READ_ANS        =                 0x02;

//  - --- - --- - --- - --- -

cchar* get_fmode(uint8_t mode)                  {

    if     (mode == DAF_WRITE )                 { return "wb+";                                                         }
    else if(mode == DAF_READ )                  { return "rb";                                                          }
    else if(mode == DAF_APPEND )                { return "ab+";                                                         }
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

struct DSM_ARCHIVE_FORMAT {

    ushort   fileCount;
    uint32_t size;
    uint32_t offsets[256];
    uchar*   data;

};

DAF emptyarch = {0};

typedef struct MESH_FILE_3D                     {

    ushort   size;
    ushort   vertCount;
    ushort   indexCount;

    pVP3D_8* bounds;
    VP3D_8*  verts;
    ushort*  indices;

} CrkFile;

typedef struct IMAGE_FILE_8BIT_RGB              {

    uint    size;
    ushort  height;
    ushort  width;

    ushort* pixels;

} JojFile;

void del_CrkFile(CrkFile* crk)                  {

    WARD_EVIL_MFREE(crk->bounds);
    WARD_EVIL_MFREE(crk->verts);
    WARD_EVIL_MFREE(crk->indices);                                                                                      }
void del_JojFile(JojFile* joj)                  { WARD_EVIL_MFREE(joj->pixels);                                         }
//  - --- - --- - --- - --- -

int openarch(cchar*        filename,
             cchar*        readmode,
             cchar         archtype[],
             const uint8_t mode,
             DAF*          daf,
             int           shutit)              {

    int isnew = 0, errorstate = 0;
    daf->fileCount = 0;
    daf->size = 8 + 2 + 4 + 1024;

    isnew = openbin(filename, readmode, shutit);
    WARD_EVIL_WRAP(errorstate, (isnew > -1) ? isnew : errorstate);

    if(isnew || (mode == DAF_WRITE
    && mode != DAF_READ) )                      { for(uint i = 1; i < 256; i++) { daf->offsets[i] = 0; }

                                                  EVIL_FWRITE(cchar,    8,   archtype,        filename);
                                                  EVIL_FWRITE(ushort,   1,   &daf->fileCount, filename);
                                                  EVIL_FWRITE(uint32_t, 1,   &daf->size,      filename);
                                                  EVIL_FWRITE(uint32_t, 256, daf->offsets,    filename);
                                                  fseek(curfile, 0, SEEK_CUR);                                          }

//  - --- - --- - --- - --- -

    else
    {

        if(isnew && mode == DAF_READ)           { fprintf(stderr, "Archive %s does not exist.\n", filename);
                                                  WARD_EVIL_WRAP(errorstate, remove(filename)); return ERROR;           }

        if(mode == DAF_APPEND)                  { rewind(curfile);                                                      }

        char sign[8];
        fread(sign, sizeof(char), 8, curfile);

        for(uint i = 0; i < 8; i++)             { if(sign[i] != archtype[i])
                                                { terminator(0x67, filename); return ERROR; }                           }

        printf("Good enough\n");

        fread(&daf->fileCount, sizeof(ushort), 1, curfile);
        fread(&daf->size,      sizeof(uint32_t), 1,   curfile);
        fread(daf->offsets,    sizeof(uint32_t), 256, curfile);

        uint32_t datasize = daf->size - DAF_HSIZE;

        if((mode == DAF_UPDATE)
        || (mode == DAF_DELETE))                { EVIL_FREAD(uchar, datasize, daf->data);
                                                  fseek(curfile, 0, SEEK_CUR); rewind(curfile);                         }

    }
    return 0;                                                                                                           }

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
    EVIL_FREAD(float,  crk->vertCount * 5,  verts);
    EVIL_FREAD(ushort, crk->indexCount * 3, crk->indices);

    WARD_EVIL_WRAP(errorstate, closebin(filename, 0));

//  - --- - --- - --- - --- -

    crk->size   = 4 + (8 * 3) + (crk->vertCount * 4) + (crk->indexCount * 2);
    crk->bounds = (pVP3D_8*) evil_malloc(8,      sizeof(pVP3D_8));
    crk->verts  = (VP3D_8*)  evil_malloc(*sizes, sizeof(VP3D_8) );

    for(uint i = 0, j = 0;
        i < (uint) crk->vertCount * 5;
        i+= 5, j++ )                            { *(crk->verts+j) =  build_vertpacked_3d_8bit(verts+i);                 }

    for(uint i = 0, j = 0;
        i < 24; i+= 3, j++ )                    { *(crk->bounds+j) = build_physvert_3d_8bit(bounds+i);                  }

    WARD_EVIL_WRAP(errorstate, remove(filename));
    printf("Deleted file <%s>\n", filename);

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int read_jojdump(cchar*   filename,
                 JojFile* joj,
                 ushort*  colbuff,
                 float*   pixels)               {

    int errorstate = 0;
    WARD_EVIL_WRAP(errorstate, openbin(filename, "rb", 0));

    fread(&joj->width,  sizeof(ushort), 1, curfile);
    fread(&joj->height, sizeof(ushort), 1, curfile);

    EVIL_FREAD(float, joj->height * joj->width * 4, pixels);

    WARD_EVIL_WRAP(errorstate, closebin(filename, 0));

    uchar   curcol      = 0;
    uchar   lastcol     = 199;
    uchar   colcount    = 1;

    ushort  colorpacked = 0;

    uint    dim         = joj->width * joj->height;
    uint j              = 0;

    colbuff             = (ushort*) evil_malloc(dim, sizeof(ushort));

    for(uint i = 0; i < dim * 4; i+=4)
    {

        curcol = color_to_joj8(pixels[i+0],
                               pixels[i+1],
                               pixels[i+2]);

        if  ( (curcol != lastcol)
            ||(colcount == 255  ) )             { colorpacked = (curcol) + (colcount << 8);
                                                  colbuff[j] = colorpacked; j++; colcount = 1;                          }

        else                                    { colcount++;                                                           }

        lastcol = curcol;

    }

    joj->size   = j;
    joj->pixels = (ushort*) evil_malloc(j, sizeof(ushort));

    for(uint i = 0; i < j; i++)                 { joj->pixels[i] = colbuff[i];                                          }

    WARD_EVIL_WRAP(errorstate, remove(filename));
    printf("Deleted file <%s>\n", filename);

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int crk_to_daf(CrkFile*    crk,
               cchar* filename)                 {

    ushort sizes[2] = { crk->vertCount, crk->indexCount};

    fseek(curfile, 0, SEEK_CUR);
    EVIL_FWRITE(ushort,  2,               sizes,        filename);
    EVIL_FWRITE(pVP3D_8, 8,               crk->bounds,  filename);
    EVIL_FWRITE(VP3D_8,  crk->vertCount,  crk->verts,   filename);
    EVIL_FWRITE(ushort,  crk->indexCount, crk->indices, filename);
    fseek(curfile, 0, SEEK_CUR);

    return 0;                                                                                                           }

int joj_to_daf(JojFile*    joj,
               cchar* filename)                 {

    ushort sizes[2] = { joj->width, joj->height};

    fseek(curfile, 0, SEEK_CUR);
    EVIL_FWRITE(ushort, 2,         sizes,       filename);
    EVIL_FWRITE(uint,   1,         &joj->size,  filename);
    EVIL_FWRITE(ushort, joj->size, joj->pixels, filename);
    fseek(curfile, 0, SEEK_CUR);

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int writecrk_daf(CrkFile*    crk,
                 DAF*        daf,
                 uint8_t     mode,
                 uint8_t     offset,
                 cchar*     filename)           {

    int evilstate = 0;

    {
        cchar* readmode = get_fmode(mode);
        WARD_EVIL_WRAP(evilstate, openarch(filename,
                                           readmode,
                                           CRKSIGN,
                                           mode,
                                           daf,
                                           0        ));
    }

    if      (daf->fileCount == 256
            && mode != DAF_UPDATE)              { printf("Archive <%s> is full; addition aborted.\n", filename);
                                                  return 0;                                                             }

    if      (mode == DAF_WRITE)
    {
        WARD_EVIL_WRAP(evilstate, crk_to_daf(crk, filename));
        daf->fileCount++;

        rewind(curfile);
        fseek (curfile, 8, SEEK_CUR);
        fseek (curfile, 0, SEEK_CUR);

        uint32_t newsize_offset[2] = { daf->size + crk->size, DAF_HSIZE };

        EVIL_FWRITE(ushort,   1, &daf->fileCount, filename);
        EVIL_FWRITE(uint32_t, 2, newsize_offset,  filename);

    }

    else if (mode == DAF_APPEND)
    {
        WARD_EVIL_WRAP(evilstate, crk_to_daf(crk, filename));

        closebin(filename, 1);
        openbin (filename, "rb+", 1);

        fseek (curfile, 8, SEEK_CUR);
        fseek (curfile, 0, SEEK_CUR);

        uint32_t newsize = daf->size + crk->size;
        uint32_t newoffset = daf->size;
        uint16_t offset_stride = (daf->fileCount) * 4;

        daf->fileCount++;
        EVIL_FWRITE(ushort,   1, &daf->fileCount,  filename);
        EVIL_FWRITE(uint32_t, 1, &newsize,         filename);

        fseek (curfile, 0,             SEEK_CUR);
        fseek (curfile, offset_stride, SEEK_CUR);

        EVIL_FWRITE(uint32_t, 1, &newoffset,       filename);
    }

    else if (mode == DAF_UPDATE && offset < 256)
    {

        int isLastChunk = offset == 255 || offset == (daf->fileCount - 1);

        uint32_t chunk_start = daf->offsets[offset];
        uint32_t newsize, byteshift, old_end;

        if(isLastChunk)
        {
            newsize   = chunk_start + crk->size;
            old_end   = daf->size;
            byteshift = newsize - old_end;

            fseek (curfile, 0,           SEEK_CUR);
            fseek (curfile, chunk_start, SEEK_CUR);
            WARD_EVIL_WRAP(evilstate, crk_to_daf(crk, filename));
            fseek (curfile, 0,           SEEK_CUR);

            rewind(curfile);
            fseek(curfile, 10, SEEK_CUR);
            fseek(curfile, 0, SEEK_CUR);
            EVIL_FWRITE(uint32_t, 1, &newsize, filename);
            fseek(curfile, 0, SEEK_CUR);
        }

        else
        {
            uint32_t chunk_end;

            chunk_end = chunk_start + crk->size;
            old_end   = daf->offsets[offset+1];
            byteshift = chunk_end - old_end;
            newsize   = (daf->size - (old_end - chunk_start)) + crk->size;

            fseek(curfile, 0,           SEEK_CUR);
            fseek(curfile, chunk_start, SEEK_CUR);
            WARD_EVIL_WRAP(evilstate, crk_to_daf(crk, filename));

            daf->data += (old_end - DAF_HSIZE);
            fseek(curfile, 0,         SEEK_CUR);
            fseek(curfile, chunk_end, SEEK_CUR);
            fseek(curfile, 0,         SEEK_CUR);
            EVIL_FWRITE(uchar, daf->size - old_end, daf->data, filename);
            fseek(curfile, 0,         SEEK_CUR);

            for(uint i = offset+1;
                i < daf->fileCount; i++)        { daf->offsets[i] += byteshift;                                         }

            rewind(curfile);
            fseek(curfile, 10, SEEK_CUR);
            fseek(curfile, 0, SEEK_CUR);
            EVIL_FWRITE(uint32_t, 1,   &newsize,     filename);
            EVIL_FWRITE(uint32_t, 256, daf->offsets, filename);
            fseek(curfile, 0, SEEK_CUR);
        }

        _chsize_s(_fileno(curfile), daf->size + byteshift);

    }

    closebin(filename, 0);
    return 0;
                                                                                                                        }

//  - --- - --- - --- - --- -

int writejoj_daf(JojFile*    joj,
                 DAF*        daf,
                 uint8_t     mode,
                 uint8_t     offset,
                 cchar*     filename)           {

    int evilstate = 0;

    {
        cchar* readmode = get_fmode(mode);
        WARD_EVIL_WRAP(evilstate, openarch(filename,
                                           readmode,
                                           JOJSIGN,
                                           mode,
                                           daf,
                                           0        ));
    }

    if      (daf->fileCount == 256
            && mode != DAF_UPDATE)              { printf("Archive <%s> is full; addition aborted.\n", filename);
                                                  return 0;                                                             }

    if      (mode == DAF_WRITE)
    {
        WARD_EVIL_WRAP(evilstate, joj_to_daf(joj, filename));
        daf->fileCount++;

        rewind(curfile);
        fseek (curfile, 8, SEEK_CUR);
        fseek (curfile, 0, SEEK_CUR);

        uint32_t newsize_offset[2] = { daf->size + joj->size, DAF_HSIZE };

        EVIL_FWRITE(ushort,   1, &daf->fileCount, filename);
        EVIL_FWRITE(uint32_t, 2, newsize_offset,  filename);

    }

    else if (mode == DAF_APPEND)
    {
        WARD_EVIL_WRAP(evilstate, joj_to_daf(joj, filename));

        closebin(filename, 1);
        openbin (filename, "rb+", 1);

        fseek (curfile, 8, SEEK_CUR);
        fseek (curfile, 0, SEEK_CUR);

        uint32_t newsize = daf->size + joj->size;
        uint32_t newoffset = daf->size;
        uint16_t offset_stride = (daf->fileCount) * 4;

        daf->fileCount++;
        EVIL_FWRITE(ushort,   1, &daf->fileCount,  filename);
        EVIL_FWRITE(uint32_t, 1, &newsize,         filename);

        fseek (curfile, 0,             SEEK_CUR);
        fseek (curfile, offset_stride, SEEK_CUR);

        EVIL_FWRITE(uint32_t, 1, &newoffset,       filename);
    }

    else if (mode == DAF_UPDATE && offset < 256)
    {

        int isLastChunk = offset == 255 || offset == (daf->fileCount - 1);

        uint32_t chunk_start = daf->offsets[offset];
        uint32_t newsize, byteshift, old_end;

        if(isLastChunk)
        {
            newsize   = chunk_start + joj->size;
            old_end   = daf->size;
            byteshift = newsize - old_end;

            fseek (curfile, 0,           SEEK_CUR);
            fseek (curfile, chunk_start, SEEK_CUR);
            WARD_EVIL_WRAP(evilstate, joj_to_daf(joj, filename));
            fseek (curfile, 0,           SEEK_CUR);

            rewind(curfile);
            fseek(curfile, 10, SEEK_CUR);
            fseek(curfile, 0, SEEK_CUR);
            EVIL_FWRITE(uint32_t, 1, &newsize, filename);
            fseek(curfile, 0, SEEK_CUR);
        }

        else
        {
            uint32_t chunk_end;

            chunk_end = chunk_start + joj->size;
            old_end   = daf->offsets[offset+1];
            byteshift = chunk_end - old_end;
            newsize   = (daf->size - (old_end - chunk_start)) + joj->size;

            fseek(curfile, 0,           SEEK_CUR);
            fseek(curfile, chunk_start, SEEK_CUR);
            WARD_EVIL_WRAP(evilstate, joj_to_daf(joj, filename));

            daf->data += (old_end - DAF_HSIZE);
            fseek(curfile, 0,         SEEK_CUR);
            fseek(curfile, chunk_end, SEEK_CUR);
            fseek(curfile, 0,         SEEK_CUR);
            EVIL_FWRITE(uchar, daf->size - old_end, daf->data, filename);
            fseek(curfile, 0,         SEEK_CUR);

            for(uint i = offset+1;
                i < daf->fileCount; i++)        { daf->offsets[i] += byteshift;                                         }

            rewind(curfile);
            fseek(curfile, 10, SEEK_CUR);
            fseek(curfile, 0, SEEK_CUR);
            EVIL_FWRITE(uint32_t, 1,   &newsize,     filename);
            EVIL_FWRITE(uint32_t, 256, daf->offsets, filename);
            fseek(curfile, 0, SEEK_CUR);
        }

        _chsize_s(_fileno(curfile), daf->size + byteshift);

    }

    closebin(filename, 0);
    return 0;
                                                                                                                        }

//  - --- - --- - --- - --- -

int popirf(DAF*    daf,
           uint8_t offset,
           cchar*  filename)                {

    int evilstate = 0;

    {
        WARD_EVIL_WRAP(evilstate, openarch(filename,
                                           "rb+",
                                           CRKSIGN,
                                           4,
                                           daf,
                                           0        ));
    }

    if      (!daf->fileCount)                   { printf("Archive <%s> is empty; deletion aborted.\n", filename);
                                                  return 0;                                                             }

    else if (offset < 256)
    {

        int isLastChunk = offset == 255 || offset == (daf->fileCount - 1);

        uint32_t chunk_start = daf->offsets[offset];
        uint32_t byteshift, newsize;

        if(isLastChunk)
        {
            byteshift = daf->size - chunk_start;
            newsize   = daf->size - byteshift;

            daf->offsets[offset] = 0;
            daf->fileCount--;

            fseek(curfile, 8, SEEK_CUR);
            fseek(curfile, 0, SEEK_CUR);
            EVIL_FWRITE(uint32_t, 1,   &daf->fileCount, filename);
            EVIL_FWRITE(uint32_t, 1,   &newsize,        filename);
            EVIL_FWRITE(uint32_t, 256, daf->offsets,    filename);
            fseek(curfile, 0, SEEK_CUR);
        }

        else
        {
            uint32_t old_end = daf->offsets[offset+1];

            byteshift = old_end - chunk_start;
            newsize   = daf->size - byteshift;
            daf->fileCount--;

            daf->data += (old_end - DAF_HSIZE);
            fseek(curfile, 0,           SEEK_CUR);
            fseek(curfile, chunk_start, SEEK_CUR);
            fseek(curfile, 0,           SEEK_CUR);
            EVIL_FWRITE(uchar, daf->size - old_end, daf->data, filename);
            fseek(curfile, 0,           SEEK_CUR);

            for(uint i = offset;
                i < daf->fileCount; i++)        { daf->offsets[i] = daf->offsets[i+1];                                  }

            daf->offsets[daf->fileCount] = 0;

            rewind(curfile);
            fseek(curfile, 8, SEEK_CUR);
            fseek(curfile, 0, SEEK_CUR);
            EVIL_FWRITE(uint32_t, 1,   &daf->fileCount, filename);
            EVIL_FWRITE(uint32_t, 1,   &newsize,        filename);
            EVIL_FWRITE(uint32_t, 256, daf->offsets,    filename);
            fseek(curfile, 0, SEEK_CUR);
        }

        _chsize_s(_fileno(curfile), daf->size + byteshift);
    }

    return 0;
                                                                                                                        }
//  - --- - --- - --- - --- -

int writecrk(cchar* filename,
             cchar* archive,
             char*  mode,
             char*  offset)                     {

    int evilstate =    0;

    DAF daf     =     {0};
    CrkFile crk =     {0};

    float* bounds =    NULL;
    float* verts  =    NULL;

    uint8_t numoffset = (uint8_t) hexstr_tolong(offset);
    uint8_t nummode   = (uint8_t) hexstr_tolong(mode);

    zjc_convertor_init(BUILD_FRAC8);
    evilstate = read_crkdump(filename, &crk, bounds, verts);

    zjc_convertor_end();

    WARD_EVIL_MFREE(bounds);
    WARD_EVIL_MFREE(verts);

    if(!evilstate)                              { evilstate = writecrk_daf(&crk, &daf, nummode, numoffset, archive);    }
    if(nummode == DAF_UPDATE || DAF_DELETE)     { WARD_EVIL_MFREE(daf.data);                                            }

    del_CrkFile(&crk);

    return evilstate;                                                                                                   }

//  - --- - --- - --- - --- -

int writejoj(cchar* filename,
             cchar* archive,
             char*  mode,
             char*  offset)                     {

    int evilstate     = 0;

    DAF     daf       = {0};
    JojFile joj       = {0};

    ushort* colbuff   = NULL;
    float*  pixels    = NULL;

    uint8_t numoffset = (uint8_t) hexstr_tolong(offset);
    uint8_t nummode   = (uint8_t) hexstr_tolong(mode);

    zjc_convertor_init(BUILD_JOJ8);
    evilstate = read_jojdump(filename, &joj, colbuff, pixels);

    zjc_convertor_end();

    WARD_EVIL_MFREE(colbuff);
    WARD_EVIL_MFREE(pixels);

    if(!evilstate)                              { evilstate = writejoj_daf(&joj, &daf, nummode, numoffset, archive);    }
    if(nummode == DAF_UPDATE || DAF_DELETE)     { WARD_EVIL_MFREE(daf.data);                                            }

    del_JojFile(&joj);

    return evilstate;                                                                                                   }

//  - --- - --- - --- - --- -

cchar* get_archtype(uint8_t archtype)           {

    if     (archtype == 0)                           { return CRKSIGN;                                                  }
    else if(archtype == 1)                           { return JOJSIGN;                                                  }
    else                                             { return FBGM;                                                     }
                                                                                                                        }

int    extraction_start (cchar*  filename,
                         uchar   archtype,
                         DAF**    daf)           {

    int evilstate   = 0;
    cchar* sign     = get_archtype(archtype);
    cchar* readmode = get_fmode(DAF_READ);
    WARD_EVIL_WRAP(evilstate, openarch(filename,
                                       readmode,
                                       sign,
                                       DAF_READ,
                                       *daf,
                                       0        ));

    return 0;                                                                                                           }

int    extraction_end   (cchar* filename,
                         DAF**  daf)            {

    closebin(filename, 0);
    **daf = emptyarch;

    return 0;                                                                                                           }

int extractcrk (DAF*     daf,
                uchar    offset,
                ushort*  vertCount,
                ushort*  indexCount,
                pVP3D_8* bounds,
                VP3D_8*  verts,
                ushort*  indices)               {

    rewind(curfile);
    fseek(curfile, 0, SEEK_CUR);
    fseek(curfile, daf->offsets[offset], SEEK_CUR);
    fseek(curfile, 0, SEEK_CUR);

    fread(vertCount,  sizeof(ushort), 1, curfile);
    fread(indexCount, sizeof(ushort), 1, curfile);
    
    EVIL_FREAD(pVP3D_8, 8,           bounds);
    EVIL_FREAD(VP3D_8,  *vertCount,  verts);
    EVIL_FREAD(ushort,  *indexCount, indices);

    return 0;                                                                                                           }

int    extractjoj (DAF*    daf,
                   uchar   offset,
                   uint*   size,
                   ushort* width,
                   ushort* height,
                   ushort* pixels)              {

    rewind(curfile);
    fseek(curfile, 0, SEEK_CUR);
    fseek(curfile, daf->offsets[offset], SEEK_CUR);
    fseek(curfile, 0, SEEK_CUR);

    fread(width,   sizeof(ushort),  1, curfile);
    fread(height,  sizeof(ushort),  1, curfile);
    fread(size,    sizeof(uint),    1, curfile);

    EVIL_FREAD(ushort, *size, pixels);

    return 0;                                                                                                           }

