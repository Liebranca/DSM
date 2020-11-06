#include <stdio.h>
#include <stdint.h>
#include <io.h>

#include "ZJC_FileHandling.h"
#include "ZJC_BitPacker.h"

#include "../lymath/ZJC_GOPS.h"
#include "../lyutils/ZJC_Evil.h"

#include <string.h>
#include "zlib.h"

//  - --- - --- - --- - --- -

#define CHUNK 0x4000

#define CALL_ZLIB(x) {                                                  \
        int status;                                                     \
        status = x;                                                     \
        if (status < 0) {                                               \
            fprintf (stderr,                                            \
                     "%s:%d: %s returned a bad status of %d.\n",        \
                     __FILE__, __LINE__, #x, status);                   \
            /*exit (EXIT_FAILURE);*/                                    \
        }                                                               \
    }

#define windowBits 15
#define GZIP_ENCODING 16

static void defstrm_init (z_stream * strm)
{
    strm->zalloc = Z_NULL;
    strm->zfree  = Z_NULL;
    strm->opaque = Z_NULL;
    CALL_ZLIB (deflateInit2 (strm, Z_BEST_COMPRESSION, Z_DEFLATED,
                             windowBits | GZIP_ENCODING, 8,
                             Z_DEFAULT_STRATEGY));
}

int INFDAF(uchar* source, uchar* unpckd, uint size_d, uint size_i) {

    z_stream strm  = {0};

    strm.total_in  = strm.avail_in  = size_d;
    strm.total_out = strm.avail_out = size_i;
    strm.next_in   = source;
    strm.next_out  = unpckd;

    strm.zalloc = Z_NULL;
    strm.zfree  = Z_NULL;
    strm.opaque = Z_NULL;

    int err = -1;
    int ret = -1;

    err = inflateInit2(&strm, (15 + 32));
    if (err == Z_OK) {
        err = inflate(&strm, Z_FINISH);
        if (err == Z_STREAM_END) {
            ret = strm.total_out;
        }
        else {
             inflateEnd(&strm);
             return err;
        }
    }
    else {
        inflateEnd(&strm);
        return err;
    }

    inflateEnd(&strm);
    return ret;
}

int DEFLDAF(uchar* source, uchar* pckd, uint size_i, uint* size_d)
{
    uchar out[CHUNK];
    z_stream strm;

    defstrm_init(& strm);
    strm.next_in  = source;
    strm.avail_in = size_i;

    do {
        int have;
        strm.avail_out = CHUNK;
        strm.next_out = out;
        CALL_ZLIB (deflate (& strm, Z_NO_FLUSH));
        have = CHUNK - strm.avail_out;

        memcpy(pckd + (*size_d), source, have); (*size_d) += have;

    }
    while (strm.avail_out == 0);
    deflateEnd (& strm);

    printf("DEFLDAF: %u/%u | approx. %u%% file size reduction\n",

           *size_d, size_i,

           (uint) (100.0 - (( (float) ( *size_d) / (float) (size_i) ) * 100))

          );

    return 0;
}

//  - --- - --- - --- - --- -

#define EVIL_FREAD(type, count, buff)            buff = (type*) evil_malloc(count, sizeof(type));\
                                                 fread(buff, sizeof(type), count, curfile);

#define EVIL_FWRITE(type, count, buff, info)     WARD_EVIL_FWRITE(fwrite(buff, sizeof(type),\
                                                 count, curfile), info, count);

//  - --- - --- - --- - --- -

static FILE*    curfile;
static H8PACK   curhpck         =                {0};
static uchar*   hpckdata;

cushort         DAF_HSIZE       =                 0x40e;

const uint8_t   DAF_WRITE       =                 0x00;
const uint8_t   DAF_APPEND      =                 0x01;
const uint8_t   DAF_UPDATE      =                 0x02;
const uint8_t   DAF_READ        =                 0x03;
const uint8_t   DAF_DELETE      =                 0x04;

cuint           MAXSTRIDE       =                 0xFFFFFFFF;

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

struct DARK_AGE_FILE {

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

typedef struct TEXTURE_FILE_24BITYUVA           {

    uint    size;
    uint    datasize_i;
    uint    datasize_d;

    ushort  height;
    ushort  width;

    uchar   imcount;
    uchar*  data;

} JOJ;

typedef struct JOJ_MATERIAL_BLOCK {

    float   spec_mult;
    float   diff_mult;
    float   ref_mult;
    float   glow_rea;

    ushort  shdid;
    ushort  flags;

} JOJMATE;

const uint JDAF_MATEBLOCK = sizeof(JOJMATE) * ZJC_DAFSIZE;

void del_CrkFile(CRK* crk)                      {

    WARD_EVIL_MFREE(crk->bounds);
    WARD_EVIL_MFREE(crk->verts);
    WARD_EVIL_MFREE(crk->indices);                                                                                      }

void del_JojFile(JOJ* joj)                      { WARD_EVIL_MFREE(joj->data);                                           }

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

    int isnew        = 0;
    int errorstate   = 0;

    daf->fileCount   = 0;
    daf->size        = 8 + 2 + 4 + (sizeof(daf->offsets[0]) * ZJC_DAFSIZE);

    int hasMateBlock = !strcmp                  (archtype, JOJSIGN                                                      );
    isnew            =  openbin                 (filename, readmode, shutit                                             );

//  - --- - --- - --- - --- -

    if(isnew)
    {
        for(uint i = 1; i < ZJC_DAFSIZE; i++)   { daf->offsets[i] = 0;                                                  }

       EVIL_FWRITE                              (cchar,    8,           archtype,        filename                       );
       EVIL_FWRITE                              (ushort,   1,           &daf->fileCount, filename                       );
       EVIL_FWRITE                              (uint32_t, 1,           &daf->size,      filename                       );
       EVIL_FWRITE                              (uint32_t, ZJC_DAFSIZE, daf->offsets,    filename                       );

       if(hasMateBlock)
       {
           daf->size += JDAF_MATEBLOCK;

           JOJMATE mateblock[ZJC_DAFSIZE] =     { 0                                                                     };
           EVIL_FWRITE                          (JOJMATE,  ZJC_DAFSIZE, mateblock,       filename                       );
       }

       fseek                                    (curfile,  0,           SEEK_CUR                                        );
    }

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

int   zh8_write    (H8PACK* hpck,
                     cchar* filename)           {

    uint intvalues [2] = { hpck->datasize,            hpck->usize              };

    EVIL_FWRITE          (ushort, 1,                  &hpck->dictsize, filename);
    EVIL_FWRITE          (uint,   2,                  intvalues,       filename);
    EVIL_FWRITE          (uchar,  hpck->dictsize,     hpck->dict,      filename);
    EVIL_FWRITE          (uchar,  hpck->datasize,     hpck->data,      filename);

    return 0;                                                                                                           }

void  zh8_read      (H8PACK* hpck)              {

    fread(&hpck->dictsize, sizeof(ushort), 1, curfile);
    fread(&hpck->datasize, sizeof(uint ),  1, curfile);
    fread(&hpck->usize,    sizeof(uint ),  1, curfile);

    EVIL_FREAD(uchar, hpck->dictsize, hpck->dict     );
    EVIL_FREAD(uchar, hpck->datasize, hpck->data     );                                                                 }

//  - --- - --- - --- - --- -

void joj_texPack   (JOJ*   joj,
                    uint   dim,
                    float* pixels)              {

    JOJPIX curcol    = { 0 };
    uchar* imdata    = (uchar*) evil_malloc(dim * joj->imcount * 4, sizeof(uchar));

//  - --- - --- - --- - --- -

    for(uint i = 0, j = 0; i < joj->datasize_i; i += 4, j++)
    {

        curcol                                 = rgba_to_joj(pixels[i+0],
                                                             pixels[i+1],
                                                             pixels[i+2],
                                                             pixels[i+3]);

        imdata[j                             ] = curcol.luma;
        imdata[j + (dim * joj->imcount      )] = curcol.chroma_u;
        imdata[j + ((dim * 2) * joj->imcount)] = curcol.chroma_v;
        imdata[j + ((dim * 3) * joj->imcount)] = curcol.alpha;

    }

//  - --- - --- - --- - --- -

    joj->data = (uchar*) evil_malloc(joj->datasize_i, sizeof(uchar));
    DEFLDAF(imdata, joj->data, joj->datasize_i, &joj->datasize_d);

    WARD_EVIL_MFREE(imdata);

    joj->size += joj->datasize_d;

    }

void joj_subTexRead (uint    dim,
                     uchar   imcount,
                     uchar   curim,
                     float** pixels   )         {

    for(uint i = (dim * curim), j = 0; i < (dim * (curim + 1)); i++, j += 4)
    {
        JOJPIX pix = { hpckdata[i + (dim * imcount      )],     // chroma_u
                       hpckdata[i + ((dim * 2) * imcount)],     // chroma_v
                       hpckdata[i                        ],     // luma
                       hpckdata[i + ((dim * 3) * imcount)]  };  // alpha

        joj_to_rgba( (*pixels)+j, &pix );
    }
                                                                                                                        }

//  - --- - --- - --- - --- -

int read_jojdump(cchar*   filename,
                 JOJ*     joj,
                 JOJMATE* mate,
                 uchar*   mateoff   )           {

    int errorstate = 0;
    WARD_EVIL_WRAP(errorstate, openbin(filename, "rb", 0));

    float fvalues[4] = {0, 0, 0, 0};

    fread(&joj->width,   sizeof(ushort), 1, curfile);
    fread(&joj->height,  sizeof(ushort), 1, curfile);
    fread(&joj->imcount, sizeof(uchar ), 1, curfile);
    fread(&mate->flags,  sizeof(uchar ), 1, curfile);
    fread(fvalues,       sizeof(float ), 4, curfile);
    fread(&mate->shdid,  sizeof(ushort), 1, curfile);
    fread(mateoff,       sizeof(uchar ), 1, curfile);

    mate->spec_mult    = fvalues[0];
    mate->diff_mult    = fvalues[1];
    mate->ref_mult     = fvalues[2];
    mate->glow_rea     = fvalues[3];

    uint dim           = joj->width * joj->height;
    joj->datasize_i    = dim * joj->imcount * 4;

    float* pixels;

    EVIL_FREAD    (float, joj->datasize_i, pixels   );
    WARD_EVIL_WRAP(errorstate, closebin(filename, 0));

    joj->size          = 17;
    joj->datasize_d    =  0;

    joj_texPack(joj, dim, pixels);

    WARD_EVIL_MFREE(pixels);
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

int joj_to_daf(JOJ*     joj,
               cchar*   filename)               {

    uint   ivalues[3] = { joj->size,       joj->datasize_i, joj->datasize_d };
    ushort svalues[2] = { joj->width,      joj->height                      };

    fseek      (curfile, 0,               SEEK_CUR               );

    EVIL_FWRITE(uint,    3,               ivalues,       filename);
    EVIL_FWRITE(ushort,  2,               svalues,       filename);
    EVIL_FWRITE(uchar,   1,               &joj->imcount, filename);
    EVIL_FWRITE(uchar,   joj->datasize_d, joj->data,     filename);

    fseek      (curfile, 0,               SEEK_CUR               );

    return 0;                                                                                                           }

int jojmate_to_daf(JOJMATE* mate,
                   cchar*   filename)           {

    float  fvalues[4] = { mate->spec_mult, mate->diff_mult, mate->ref_mult, mate->glow_rea };
    ushort svalues[2] = { mate->shdid,     mate->flags                                     };

    fseek               (curfile, 0,       SEEK_CUR                                        );

    EVIL_FWRITE         (float,   4,       fvalues, filename                               );
    EVIL_FWRITE         (ushort,  2,       svalues, filename                               );

    fseek               (curfile, 0,       SEEK_CUR                                        );

    printf(" %f, %f, %f, %f | %u, %u | FUK\n",

           fvalues[0], fvalues[1], fvalues[2], fvalues[3],
           svalues[0], svalues[1]                           );

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
                 JOJMATE*   mate,
                 uchar      mateoff,
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

        rewind(curfile             );
        fseek (curfile, 8, SEEK_CUR);
        fseek (curfile, 0, SEEK_CUR);

        uint32_t newsize_offset[2] = { daf->size + joj->size, DAF_HSIZE + JDAF_MATEBLOCK };

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

    if(!(mate->flags & 128))                    { rewind        (curfile                                  );
                                                  fseek         (curfile,   DAF_HSIZE,    SEEK_CUR        );
                                                  fseek         (curfile,   0,            SEEK_CUR        );

                                                  uint first_stride = sizeof(JOJMATE) * mateoff;
                                                  uint final_stride = JDAF_MATEBLOCK - (first_stride + sizeof(JOJMATE));

                                                  fseek         (curfile,   0,            SEEK_CUR        );
                                                  fseek         (curfile,   first_stride, SEEK_CUR        );

                                                  WARD_EVIL_WRAP(evilstate, jojmate_to_daf(mate, filename));

                                                  fseek         (curfile,   final_stride, SEEK_CUR        );
                                                  fseek         (curfile,   0,            SEEK_CUR        );            }

    closebin(filename, 0);
    return 0;                                                                                                           }

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
    JOJMATE mate      = {0};
    uchar   mateoff   = 0;

    uint8_t numoffset = (uint8_t) hexstr_tolong (offset                                                                 );
    uint8_t nummode   = (uint8_t) hexstr_tolong (mode                                                                   );

    zjc_convertor_init                          (BUILD_JOJ                                                              );
    evilstate = read_jojdump                    (filename, &joj, &mate, &mateoff                                        );
    zjc_convertor_end                           (                                                                       );

    if(!evilstate)                              { evilstate = writejoj_daf(&joj, &mate, mateoff, &daf,
                                                                           nummode, numoffset, archive);                }

    if(nummode == DAF_UPDATE || DAF_DELETE)     { WARD_EVIL_MFREE(daf.data);                                            }

    del_JojFile                                 (&joj                                                                   );

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

    if(curhpck.dictsize) { WARD_EVIL_MFREE(hpckdata); zh8_delPacker(&curhpck); }

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

int    extractjoj (DAF*    daf,
                   uchar   offset,
                   uchar*  imcount,
                   uint*   size,
                   ushort* width,
                   ushort* height,
                   uchar*  flags,
                   float   fvalues[4],
                   ushort* shdid       )        {

    if(curhpck.dictsize) { WARD_EVIL_MFREE(hpckdata); zh8_delPacker(&curhpck); }

    rewind  (curfile                                );
    fseek   (curfile, 0, SEEK_CUR                   );
    fseek   (curfile, daf->offsets[offset], SEEK_CUR);
    fseek   (curfile, 0, SEEK_CUR                   );

    fread   (width,   sizeof(ushort), 1, curfile    );
    fread   (height,  sizeof(ushort), 1, curfile    );
    fread   (imcount, sizeof(uchar),  1, curfile    );
    fread   (size,    sizeof(uint),   1, curfile    );
    fread   (flags,   sizeof(uchar),  1, curfile    );
    fread   (fvalues, sizeof(float),  4, curfile    );
    fread   (shdid,   sizeof(ushort), 1, curfile    );

    zh8_read(&curhpck                               );

    hpckdata = zh8_unpack(&curhpck);

    return 0;                                                                                                           }

