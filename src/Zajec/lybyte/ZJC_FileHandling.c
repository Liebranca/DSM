#include <stdio.h>
#include <stdint.h>
#include <io.h>

#include "ZJC_FileHandling.h"
#include "ZJC_BitPacker.h"

#include "../lymath/ZJC_GOPS.h"
#include "../lyutils/ZJC_Evil.h"

#include <string.h>

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
cchar           SSXSIGN[8]      =               { 0x4c, 0x59, 0x45, 0x42, 0x24, 0x53, 0x53, 0x58                        };

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

    if      ( readmode && (curfile == NULL) )   { curfile = fopen(filename, "wb"); isnew = -1;                          }
    else if ( mode == "wb+"                 )   { isnew = 1;                                                            }

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
    uint32_t offsets[ZJC_DAFSIZE];
    uchar*   data;

};

DAF emptyarch = {0};

typedef struct MESH_FILE_3D                     {

    ushort   size;
    ushort   vertCount;
    ushort   indexCount;
    ushort   matid;

    pVP3D_8* bounds;
    VP3D_8*  verts;
    ushort*  indices;

} CRK;

typedef struct IMAGE_FILE_24BIT_YUV             {

    uint    size;
    uchar   flags;
    ushort  height;
    ushort  width;

    H8PACK  luma;
    H8PACK  chroma_u;
    H8PACK  chroma_v;

} JOJ;

void del_CrkFile(CRK* crk)                      {

    WARD_EVIL_MFREE(crk->bounds);
    WARD_EVIL_MFREE(crk->verts);
    WARD_EVIL_MFREE(crk->indices);                                                                                      }

void del_JojFile(JOJ* joj)                      { zh8_delPacker(&joj->chroma_v); zh8_delPacker(&joj->chroma_u);
                                                  zh8_delPacker(&joj->luma    );                                        }

void del_SsxFile(SSX* ssx)                      {

    for(ushort i = 0; i < ssx->count; i++)      { WARD_EVIL_MFREE(ssx->objects[i].fvalues);                             }
    WARD_EVIL_MFREE(ssx->objects);                                                                                      }

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

    // LYEB: I've changed how new files are detected and now I don't remember why I needed this line.
    //       it breaks DAF file creation so I'm commenting it out until my memory is refreshed.
    //
    // WARD_EVIL_WRAP(errorstate, (isnew > -1) ? isnew : errorstate);

    if(isnew)                                   { for(uint i = 1; i < ZJC_DAFSIZE; i++) { daf->offsets[i] = 0; }

                                                  EVIL_FWRITE(cchar,    8,           archtype,        filename);
                                                  EVIL_FWRITE(ushort,   1,           &daf->fileCount, filename);
                                                  EVIL_FWRITE(uint32_t, 1,           &daf->size,      filename);
                                                  EVIL_FWRITE(uint32_t, ZJC_DAFSIZE, daf->offsets,    filename);
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
                                                { terminator(67, filename); return ERROR; }                             }

        fread(&daf->fileCount, sizeof(ushort),   1,           curfile);
        fread(&daf->size,      sizeof(uint32_t), 1,           curfile);
        fread(daf->offsets,    sizeof(uint32_t), ZJC_DAFSIZE, curfile);

        daf->data         = NULL;
        uint32_t datasize = daf->size - DAF_HSIZE;

        if((mode == DAF_UPDATE)
        || (mode == DAF_DELETE))                { EVIL_FREAD(uchar, datasize, daf->data);
                                                  fseek(curfile, 0, SEEK_CUR); rewind(curfile);                         }

    }
    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int read_crkdump(cchar*      filename,
                 CRK*        crk,
                 float*      bounds,
                 float*      verts)             {

    int errorstate = 0;

    WARD_EVIL_WRAP(errorstate, openbin(filename, "rb", 0));

    ushort sizes[2] = {0, 0};
    fread(sizes, sizeof(ushort), 2, curfile);

    crk->vertCount  = sizes[0];
    crk->indexCount = sizes[1];

    fread(&crk->matid, sizeof(ushort), 1, curfile);

    EVIL_FREAD(float,  24,                   bounds      );
    EVIL_FREAD(float,  crk->vertCount  * 14, verts       );
    EVIL_FREAD(ushort, crk->indexCount * 3,  crk->indices);

    WARD_EVIL_WRAP(errorstate, closebin(filename, 0));

//  - --- - --- - --- - --- -

    crk->size   = 6 + (8 * 3) + (crk->vertCount * 14) + (crk->indexCount * 3 * 2);
    crk->bounds = (pVP3D_8*) evil_malloc(8,      sizeof(pVP3D_8));
    crk->verts  = (VP3D_8*)  evil_malloc(*sizes, sizeof(VP3D_8) );

    for(uint i = 0, j = 0;
        i < (uint) crk->vertCount * 14;
        i+= 14, j++ )                           { *(crk->verts+j)  = build_vertpacked_3d_8bit(verts+i);                 }

    for(uint i = 0, j = 0;
        i < 24; i+= 3, j++ )                    { *(crk->bounds+j) = build_physvert_3d_8bit(bounds+i);                  }

    WARD_EVIL_WRAP(errorstate, remove(filename));
    printf("Deleted file <%s>\n", filename);

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int read_jojdump(cchar*   filename,
                 JOJ*     joj)                  {

    int errorstate = 0;
    WARD_EVIL_WRAP(errorstate, openbin(filename, "rb", 0));

    fread(&joj->width,  sizeof(ushort), 1, curfile);
    fread(&joj->height, sizeof(ushort), 1, curfile);
    fread(&joj->flags,  sizeof(uchar),  1, curfile);

    float* pixels;
    EVIL_FREAD(float, joj->height * joj->width * 4, pixels);

    WARD_EVIL_WRAP(errorstate, closebin(filename, 0));

    JOJPIX curcol           = { 0 };

    uint dim                = joj->width * joj->height;

    joj->luma.dictsize      = 0;
    joj->luma.usize         = dim;

    joj->chroma_u.dictsize  = 0;
    joj->chroma_u.usize     = dim;

    joj->chroma_v.dictsize  = 0;
    joj->chroma_v.usize     = dim;

    uchar*  protoluma       = (uchar* ) evil_malloc(dim, sizeof(uchar ));
    uchar*  protochroma_u   = (uchar* ) evil_malloc(dim, sizeof(uchar ));
    uchar*  protochroma_v   = (uchar* ) evil_malloc(dim, sizeof(uchar ));

    uint*   lumadict        = (uint*  ) evil_malloc(256, sizeof(uint  ));
    uint*   chroma_udict    = (uint*  ) evil_malloc(256, sizeof(uint  ));
    uint*   chroma_vdict    = (uint*  ) evil_malloc(256, sizeof(uint  ));

    ushort* lumacounter     = (ushort*) evil_malloc(256, sizeof(ushort));
    ushort* chroma_ucounter = (ushort*) evil_malloc(256, sizeof(ushort));
    ushort* chroma_vcounter = (ushort*) evil_malloc(256, sizeof(ushort));

//  - --- - --- - --- - --- -

    for(uint i = 0; i < dim * 4; i+=4)
    {

        curcol        = rgb_to_joj(pixels[i+0],
                                   pixels[i+1],
                                   pixels[i+2]);

        protoluma[(uint)(i/4)] = curcol.luma;

        if  (!lumacounter[curcol.luma])         { lumadict[joj->luma.dictsize] = curcol.luma + (1 << 16);
                                                  joj->luma.dictsize++; lumacounter[curcol.luma] = joj->luma.dictsize;  }

        else                                    { ushort prev = ( lumadict[lumacounter[curcol.luma] - 1]
                                                                & 0xFFFF0000) >> 16;

                                                  lumadict[lumacounter[curcol.luma] - 1] = curcol.luma
                                                                                         + ((prev + 1) << 16);          }

//  - --- - --- - --- - --- -

        protochroma_u[(uint)(i/4)] = curcol.chroma_u;

        if  (!chroma_ucounter[curcol.chroma_u]) { chroma_udict[joj->chroma_u.dictsize] = curcol.chroma_u + (1 << 16);
                                                  joj->chroma_u.dictsize++;

                                                  chroma_ucounter[curcol.chroma_u] = joj->chroma_u.dictsize;  }

        else                                    { ushort prev = ( chroma_udict[chroma_ucounter[curcol.chroma_u] - 1]
                                                                & 0xFFFF0000) >> 16;

                                                  chroma_udict[chroma_ucounter[curcol.chroma_u] - 1] = curcol.chroma_u
                                                                                                     +((prev + 1)
                                                                                                     << 16      );      }

//  - --- - --- - --- - --- -

        protochroma_v[(uint)(i/4)] = curcol.chroma_v;

        if  (!chroma_vcounter[curcol.chroma_v]) { chroma_vdict[joj->chroma_v.dictsize] = curcol.chroma_v + (1 << 16);
                                                  joj->chroma_v.dictsize++;

                                                  chroma_vcounter[curcol.chroma_v] = joj->chroma_v.dictsize;  }

        else                                    { ushort prev = ( chroma_vdict[chroma_vcounter[curcol.chroma_v] - 1]
                                                                & 0xFFFF0000) >> 16;

                                                  chroma_vdict[chroma_vcounter[curcol.chroma_v] - 1] = curcol.chroma_v
                                                                                                     +((prev + 1)
                                                                                                     << 16      );      }

    }

//  - --- - --- - --- - --- -

    WARD_EVIL_MFREE(chroma_vcounter);
    WARD_EVIL_MFREE(chroma_ucounter);
    WARD_EVIL_MFREE(lumacounter    );

    zh8_pack(&joj->luma,     lumadict,     protoluma    );
    zh8_pack(&joj->chroma_u, chroma_udict, protochroma_u);
    zh8_pack(&joj->chroma_v, chroma_vdict, protochroma_v);

    WARD_EVIL_MFREE(chroma_vdict);
    WARD_EVIL_MFREE(chroma_udict);
    WARD_EVIL_MFREE(lumadict);

    WARD_EVIL_MFREE(protochroma_v);
    WARD_EVIL_MFREE(protochroma_u);
    WARD_EVIL_MFREE(protoluma);

    WARD_EVIL_MFREE(pixels);

//  - --- - --- - --- - --- -

    joj->size  =  9                     + 30;
    joj->size += joj->luma.dictsize     + joj->luma.datasize;
    joj->size += joj->chroma_u.dictsize + joj->chroma_u.datasize;
    joj->size += joj->chroma_v.dictsize + joj->chroma_v.datasize;

    WARD_EVIL_WRAP(errorstate, remove(filename));
    printf("Deleted file <%s>\n", filename);

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int crk_to_daf(CRK*   crk,
               cchar* filename)                 {

    ushort usvalues[3] = { crk->vertCount, crk->indexCount, crk->matid};

    fseek(curfile, 0, SEEK_CUR);
    EVIL_FWRITE(ushort,  3,                   usvalues,     filename);
    EVIL_FWRITE(pVP3D_8, 8,                   crk->bounds,  filename);
    EVIL_FWRITE(VP3D_8,  crk->vertCount,      crk->verts,   filename);
    EVIL_FWRITE(ushort,  crk->indexCount * 3, crk->indices, filename);
    fseek(curfile, 0, SEEK_CUR);

    return 0;                                                                                                           }

int joj_to_daf(JOJ*   joj,
               cchar* filename)                 {

    ushort sizes[2]    = { joj->width, joj->height};

    fseek(curfile, 0, SEEK_CUR);
    EVIL_FWRITE(ushort, 2, sizes,       filename);
    EVIL_FWRITE(uchar,  1, &joj->flags, filename);
    EVIL_FWRITE(uint,   1, &joj->size,  filename);

//  - --- - --- - --- - --- -

    uint   lumaint [2] = { joj->luma.datasize,     joj->luma.usize                };

    EVIL_FWRITE(ushort, 1,                         &joj->luma.dictsize,   filename);
    EVIL_FWRITE(uint,   2,                         lumaint,               filename);
    EVIL_FWRITE(uchar,  joj->luma.dictsize,        joj->luma.dict,        filename);
    EVIL_FWRITE(uchar,  joj->luma.datasize,        joj->luma.data,        filename);

//  - --- - --- - --- - --- -

    uint   chruint [2] = { joj->chroma_u.datasize, joj->chroma_u.usize                };

    EVIL_FWRITE(ushort, 1,                         &joj->chroma_u.dictsize,   filename);
    EVIL_FWRITE(uint,   2,                         chruint,                   filename);
    EVIL_FWRITE(uchar,  joj->chroma_u.dictsize,    joj->chroma_u.dict,        filename);
    EVIL_FWRITE(uchar,  joj->chroma_u.datasize,    joj->chroma_u.data,        filename);

//  - --- - --- - --- - --- -

    uint   chrvint [2] = { joj->chroma_v.datasize, joj->chroma_v.usize                };

    EVIL_FWRITE(ushort, 1,                         &joj->chroma_v.dictsize,   filename);
    EVIL_FWRITE(uint,   2,                         chrvint,                   filename);
    EVIL_FWRITE(uchar,  joj->chroma_v.dictsize,    joj->chroma_v.dict,        filename);
    EVIL_FWRITE(uchar,  joj->chroma_v.datasize,    joj->chroma_v.data,        filename);

//  - --- - --- - --- - --- -

    fseek(curfile, 0, SEEK_CUR);

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int writecrk_daf(CRK*       crk,
                 DAF*       daf,
                 uint8_t    mode,
                 uint8_t    offset,
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

    if      (daf->fileCount == ZJC_DAFSIZE
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

    else if (mode == DAF_UPDATE && offset < ZJC_DAFSIZE)
    {

        int isLastChunk = offset == (ZJC_DAFSIZE - 1) || offset == (daf->fileCount - 1);

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
            EVIL_FWRITE(uint32_t, ZJC_DAFSIZE, daf->offsets, filename);
            fseek(curfile, 0, SEEK_CUR);
        }

        _chsize_s(_fileno(curfile), daf->size + byteshift);

    }

    closebin(filename, 0);
    return 0;
                                                                                                                        }

//  - --- - --- - --- - --- -

int writejoj_daf(JOJ*       joj,
                 DAF*       daf,
                 uint8_t    mode,
                 uint8_t    offset,
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

    if      (daf->fileCount == ZJC_DAFSIZE
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

        uint32_t newsize   = daf->size + joj->size;
        uint32_t newoffset = daf->size;

        uint16_t offset_stride = (daf->fileCount) * 4;

        daf->fileCount++;
        EVIL_FWRITE(ushort,   1, &daf->fileCount,  filename);
        EVIL_FWRITE(uint32_t, 1, &newsize,         filename);

        fseek (curfile, 0,             SEEK_CUR);
        fseek (curfile, offset_stride, SEEK_CUR);

        EVIL_FWRITE(uint32_t, 1, &newoffset,       filename);
    }

    else if (mode == DAF_UPDATE && offset < ZJC_DAFSIZE)
    {

        int isLastChunk = offset == (ZJC_DAFSIZE - 1) || offset == (daf->fileCount - 1);

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
            EVIL_FWRITE(uint32_t, ZJC_DAFSIZE, daf->offsets, filename);
            fseek(curfile, 0, SEEK_CUR);
        }

        _chsize_s(_fileno(curfile), daf->size + byteshift);

    }

    closebin(filename, 0);
    return 0;
                                                                                                                        }

//  - --- - --- - --- - --- -

int readssx(SSX* ssx, cchar* filename)          {

    int evilstate = 0;
    WARD_EVIL_WRAP(evilstate, openbin(filename, "rb", 0));

    char sign[8];
    fread(sign, sizeof(cchar), 8, curfile);

    GETLOC;
    for(uint i = 0; i < 8; i++)                 { if(sign[i] != SSXSIGN[i])
                                                { terminator(67, filename); return ERROR; }                             }

    evil_poplocreg();

    fread(&ssx->count, sizeof(ushort), 1, curfile);

    ssx->objects = (SSO*) evil_malloc(ssx->count, sizeof(SSO));

    for(ushort i = 0; i < ssx->count; i++)
    {

        fread(&ssx->objects[i].resinfo, sizeof(ushort), 3, curfile);

        ushort flags  = ssx->objects[i].resinfo[2];
        uchar  fcount = 10;

        if(flags & 0x0004) { fcount += 12; }

        EVIL_FREAD(float, fcount, ssx->objects[i].fvalues);

    }

    WARD_EVIL_WRAP(evilstate, closebin(filename, 0));
    return 0;                                                                                                           }
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

    else if (offset < ZJC_DAFSIZE)
    {

        int isLastChunk = offset == (ZJC_DAFSIZE - 1) || offset == (daf->fileCount - 1);

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
            EVIL_FWRITE(uint32_t, ZJC_DAFSIZE, daf->offsets,    filename);
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
            EVIL_FWRITE(uint32_t, ZJC_DAFSIZE, daf->offsets,    filename);
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

    DAF daf       =    {0};
    CRK crk       =    {0};

    float* bounds =    NULL;
    float* verts  =    NULL;

    uint8_t numoffset = (uint8_t) hexstr_tolong(offset);
    uint8_t nummode   = (uint8_t) hexstr_tolong(mode);

    zjc_convertor_init(BUILD_FRAC);
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
    JOJ     joj       = {0};

    uint8_t numoffset = (uint8_t) hexstr_tolong(offset);
    uint8_t nummode   = (uint8_t) hexstr_tolong(mode);

    zjc_convertor_init(BUILD_JOJ);

    evilstate = read_jojdump(filename, &joj);

    zjc_convertor_end();

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

DAF* dafalloc()                                 { return (DAF*) evil_malloc(1, sizeof(DAF));                            }

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
    memset(*daf, 0, sizeof(**daf));

    return 0;                                                                                                           }

int extractcrk (DAF*     daf,
                uchar    offset,
                ushort*  matid,
                ushort*  vertCount,
                ushort*  indexCount,
                pVP3D_8** bounds,
                VP3D_8**  verts,
                ushort**  indices)              {

    rewind(curfile);
    fseek(curfile, 0, SEEK_CUR);
    fseek(curfile, daf->offsets[offset], SEEK_CUR);
    fseek(curfile, 0, SEEK_CUR);

    fread(vertCount,  sizeof(ushort), 1, curfile);
    fread(indexCount, sizeof(ushort), 1, curfile);
    fread(matid,      sizeof(ushort), 1, curfile);

    EVIL_FREAD(pVP3D_8, 8,               *bounds);
    EVIL_FREAD(VP3D_8,  *vertCount,      *verts);
    EVIL_FREAD(ushort,  *indexCount * 3, *indices);

    return 0;                                                                                                           }

int    extractjoj (DAF*     daf,
                   uchar    offset,
                   uchar*   flags,
                   uint*    size,
                   ushort*  width,
                   ushort*  height,
                   float** pixels)              {

    H8PACK luma     = { 0 };
    H8PACK chroma_u = { 0 };
    H8PACK chroma_v = { 0 };

    rewind(curfile);
    fseek(curfile, 0, SEEK_CUR);
    fseek(curfile, daf->offsets[offset], SEEK_CUR);
    fseek(curfile, 0, SEEK_CUR);

    fread(width,   sizeof(ushort),  1, curfile);
    fread(height,  sizeof(ushort),  1, curfile);
    fread(flags,   sizeof(uchar),   1, curfile);
    fread(size,    sizeof(uint),    1, curfile);

    uint dim = (*width) * (*height);

//  - --- - --- - --- - --- -

    fread(&luma.dictsize, sizeof(ushort), 1, curfile);
    fread(&luma.datasize, sizeof(uint ),  1, curfile);
    fread(&luma.usize,    sizeof(uint ),  1, curfile);

    EVIL_FREAD(uchar, luma.dictsize, luma.dict       );
    EVIL_FREAD(uchar, luma.datasize, luma.data       );

    uchar* luma_unpacked = zh8_unpack(&luma);

//  - --- - --- - --- - --- -

    fread(&chroma_u.dictsize, sizeof(ushort), 1, curfile);
    fread(&chroma_u.datasize, sizeof(uint ),  1, curfile);
    fread(&chroma_u.usize,    sizeof(uint ),  1, curfile);

    EVIL_FREAD(uchar, chroma_u.dictsize, chroma_u.dict       );
    EVIL_FREAD(uchar, chroma_u.datasize, chroma_u.data       );

    uchar* chroma_u_unpacked = zh8_unpack(&chroma_u);

//  - --- - --- - --- - --- -

    fread(&chroma_v.dictsize, sizeof(ushort), 1, curfile);
    fread(&chroma_v.datasize, sizeof(uint ),  1, curfile);
    fread(&chroma_v.usize,    sizeof(uint ),  1, curfile);

    EVIL_FREAD(uchar, chroma_v.dictsize, chroma_v.dict       );
    EVIL_FREAD(uchar, chroma_v.datasize, chroma_v.data       );

    uchar* chroma_v_unpacked = zh8_unpack(&chroma_v);

//  - --- - --- - --- - --- -

    *pixels = (float*) evil_malloc(dim * 3, sizeof(float));

    for(uint i = 0, j = 0; i < dim; i++)
    {
        JOJPIX pix = { chroma_u_unpacked[i], chroma_v_unpacked[i], luma_unpacked[i] };
        joj_to_rgb( (*pixels)+j, &pix ); j += 3;
    }

    WARD_EVIL_MFREE(chroma_v_unpacked);
    WARD_EVIL_MFREE(chroma_u_unpacked);
    WARD_EVIL_MFREE(luma_unpacked    );

    zh8_delPacker(&chroma_v);
    zh8_delPacker(&chroma_u);
    zh8_delPacker(&luma    );

    return 0;                                                                                                           }

