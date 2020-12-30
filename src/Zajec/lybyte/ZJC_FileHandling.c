#include <stdio.h>
#include <io.h>

#include "ZJC_FileHandling.h"

#include "../lymath/ZJC_GOPS.h"
#include "../lyutils/ZJC_Evil.h"
#include "../lyutils/ZJC_Str.h"

#include <string.h>
#include "zlib.h"

//  - --- - --- - --- - --- -

#define EVIL_FREAD(type, count, buff, bin)      WARD_EVIL_FREAD(fread(buff, sizeof(type), count, bin->file),             \
                                                                sizeof(type) * count, bin->name);

#define EVIL_FWRITE(type, count, buff, bin)     WARD_EVIL_FWRITE(fwrite(buff, sizeof(type), count, bin->file),           \
                                                                 sizeof(type)* count, bin->name);
//  - --- - --- - --- - --- -

typedef struct ZJC_BINFILE {

    cchar* name;
    FILE*  file;

} ZBIN;

//  - --- - --- - --- - --- -

#define CHUNK 0x4000

#define CALL_ZLIB(x) {                                                  \
        int status;                                                     \
        status = x;                                                     \
        if (status < 0) {                                               \
            fprintf (stderr,                                            \
                     "%s:%d: %s returned a bad status of %d.\n",        \
                     __FILE__, __LINE__, #x, status);                   \
            terminator (0, "Bruh, zLib went PLOP!");                    \
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

int ZJC_INFLATEBUFF(void* source, void* unpckd, uint size_d, uint size_i) {

    z_stream strm  = {0};

    strm.total_in  = strm.avail_in  = size_d;
    strm.total_out = strm.avail_out = size_i;
    strm.next_in   = (Bytef*) source;
    strm.next_out  = (Bytef*) unpckd;

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

int ZJC_DEFLATEBUFF(void* source, ZBIN* dst, uint size_i, uint* size_d)
{
    uchar out[CHUNK];
    z_stream strm;

    defstrm_init(&strm);
    strm.next_in  = (Bytef*) source;
    strm.avail_in = size_i;

    while (strm.avail_out == 0)
    {
        uint have;
        strm.avail_out = CHUNK;
        strm.next_out  = out;

        CALL_ZLIB (deflate (& strm, Z_NO_FLUSH));

        have           = CHUNK - strm.avail_out;
        (*size_d)     += have;

        EVIL_FWRITE(Bytef, have, out, dst);

    }
    
    deflateEnd (&strm);

    printf("DEFLDAF: %u/%u | approx. %u%% file size reduction\n",

           *size_d, size_i,

           (uint) (100.0 - (( (float) ( *size_d) / (float) (size_i) ) * 100))

          );

    return 0;                                                                                                           }

int ZJC_DEFLATEFILE(ZBIN* src, ZBIN* dst, uint blocksize, uint numblocks, uint size_i, uint* size_d)
{
    uchar out[CHUNK];
    uchar readbuff = NULL; WARD_EVIL_MALLOC(readbuff, Bytef, blocksize, 1);

    z_stream strm;

    defstrm_init(&strm);

    for(uint i = 0; i < numblocks; i++)
    {

        // read next block to compress
        fseek                                   (src->file, 0, SEEK_CUR                                                 );
        EVIL_FREAD                              (Bytef, blocksize, readbuff, src                                        );
        fseek                                   (src->file, 0, SEEK_CUR                                                 );

        strm.next_in   = readbuff;
        strm.avail_in  = blocksize;

        // compress block and write to dst
        while (strm.avail_out == 0)
        {
            uint have;
            strm.avail_out = CHUNK;
            strm.next_out  = out;

            CALL_ZLIB (deflate (& strm, Z_NO_FLUSH));

            have           = CHUNK - strm.avail_out;
            (*size_d)     += have;

            EVIL_FWRITE(Bytef, have, out, dst);

        }
    }

    deflateEnd (&strm);

    printf("DEFLATE: %u/%u | approx. %u%% file size reduction\n",

           *size_d, size_i,

           (uint) (100.0 - (( (float) ( *size_d) / (float) (size_i) ) * 100))

          );

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

static cuint           DAF_MAXSTRIDE   =          0xFFFFFFFF;
static cuint           CRK_MAXVERTS    =          0xFFFF;

static cchar           CRKSIGN[8]      =        { 0x4c, 0x59, 0x45, 0x42, 0x24, 0x43, 0x52, 0x4b                        };
static cchar           JOJSIGN[8]      =        { 0x4c, 0x59, 0x45, 0x42, 0x24, 0x4a, 0x4f, 0x4a                        };
static cchar           SSXSIGN[8]      =        { 0x4c, 0x59, 0x45, 0x42, 0x24, 0x53, 0x53, 0x58                        };

static const uint8_t   READ_MESH       =          0x00;
static const uint8_t   READ_TEX        =          0x01;
static const uint8_t   READ_ANS        =          0x02;

static const uint8_t   DAF_WRITE       =          0x00;
static const uint8_t   DAF_APPEND      =          0x01;
static const uint8_t   DAF_UPDATE      =          0x02;
static const uint8_t   DAF_READ        =          0x03;
static const uint8_t   DAF_DELETE      =          0x04;

//  - --- - --- - --- - --- -

cchar* get_fmode(uint8_t mode)                  {

    if     (mode == DAF_WRITE )                 { return "wb+";                                                         }
    else if(mode == DAF_READ )                  { return "rb";                                                          }
    else if(mode == DAF_APPEND )                { return "ab+";                                                         }
    else                                        { return "rb+";                                                         }
                                                                                                                        }

int openbin(ZBIN*  bin,
            cchar* mode,
            int    shutit)                      {

    int isnew = 0;
    bin->file = fopen(bin->name, mode);

    int readmode = (mode == "rb+") || (mode == "rb");

    if      ( readmode && (bin->file == NULL) ) { bin->file = fopen(bin->name, "wb"); isnew = -1;                       }
    else if ( mode == "wb+"                   ) { isnew     = 1;                                                        }

    WARD_EVIL_FOPEN(bin);

    if (!shutit)                                { printf("Opened file <%s>\n", bin->name);                              }
    return isnew;                                                                                                       }

//  - --- - --- - --- - --- -

int closebin(ZBIN* bin,
             int   shutit)                      {

    int success = fclose(bin->file);
    WARD_EVIL_FCLOSE(success, bin->name);

    if (!shutit)                                { printf("File closed <%s>\n", bin->name);                              }
    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

static DAF*    curdaf         = NULL;
static CRK*    curcrk         = NULL;
static JOJ*    curjoj         = NULL;

static uchar*  curdaf_data    = NULL;
static uchar*  curjoj_data    = NULL;

//  - --- - --- - --- - --- -

uint ZJC_getNext_blockSize(uint elementSize,
                           uint elementCount,
                           int* endRead     )   { uint avail = CHUNK / elementSize; uint req = elementCount * elementSize;
                                                  if(req <= CHUNK) { *endRead = 1; return elementCount; } return avail; }

int ZJC_copy_block(ZBIN* src,
                   ZBIN* dst,
                   uint  size)                  {

    uchar rblock[CHUNK];

    uint  dataleft   = size;

    while(dataleft)
    {
        uint readsize = CHUNK;
        if(readsize > dataleft) { readsize = dataleft; }

        fseek                                   (src->file, 0, SEEK_CUR                                                 );
        EVIL_FREAD                              (uchar, readsize, rblock, src                                           );
        fseek                                   (src->file, 0, SEEK_CUR                                                 );

        fseek                                   (dst->file, 0, SEEK_CUR                                                 );
        EVIL_FWRITE                             (uchar, readsize, rblock, dst                                           );
        fseek                                   (dst->file, 0, SEEK_CUR                                                 );

        dataleft -= readsize;

    }

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

void del_JojFile()                              { WARD_EVIL_MFREE(curjoj_data); WARD_EVIL_MFREE(curjoj);                }

void del_SsxFile(SSX* ssx)                      {

    for(ushort i = 0; i < ssx->count; i++)      { WARD_EVIL_MFREE(ssx->objects[i].fvalues);                             }
    WARD_EVIL_MFREE(ssx->objects);                                                                                      }

//  - --- - --- - --- - --- -

int openarch(ZBIN*         bin,
             cchar*        readmode,
             cchar         archtype[],

             const uint8_t mode,
             int           shutit)              {

    int isnew         = 0;
    int errorstate    = 0;

    isnew             =  openbin                (bin,      readmode, shutit                                             );

//  - --- - --- - --- - --- -

    if(isnew)
    {
        curdaf->fileCount                       = 0;
        curdaf->size                            = sizeof(DAF);

        strcpy                                  (curdaf->sign,    archtype                                              );
        memset                                  (curdaf->offsets, 0,           sizeof(curdaf->offsets)                  );

        EVIL_FWRITE                             (DAF,             1,           curdaf, bin                              );

        fseek                                   (bin->file,  0,     SEEK_CUR                                            );
    }

//  - --- - --- - --- - --- -

    else
    {

        if(isnew && mode == DAF_READ     )      { fprintf(stderr, "Archive %s does not exist.\n", bin->name);
                                                  WARD_EVIL_WRAP(errorstate, remove(bin->name)); return ERROR;          }

        if(mode  == DAF_APPEND           )      { rewind(bin->file);                                                    }

        fread                                   (curdaf, sizeof(DAF), 1, bin->file                                      );

        if(strcmp(curdaf->sign, archtype))      { __terminator__(68, bin->name); return ERROR;                          }

        curdaf_data                             = NULL;
        uint32_t datasize                       = curdaf->size - sizeof(DAF);

        /* This block would read the WHOLE file, which is stupid
           better go by fixed sized blocks when we actually do update/deletion and need to move data around
           but overall the entire old system is halfarsed to the max, we need to ground-up rewrite it

        if((mode == DAF_UPDATE)
        || (mode == DAF_DELETE))
        {
            EVIL_FREAD                          (uchar,   datasize, curdaf_data, bin->file                              );
            fseek                               (bin->file, 0,        SEEK_CUR                                          );
            rewind                              (bin->file                                                              );
        }*/

    }

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int ZJC_new_dafblock(uchar*  src,
                     ZBIN*   dst,
                     ZBIN*   tmpdst,

                     uint    objsize,
                     uint8_t mode,
                     uint8_t offset )           {

    if(mode == DAF_WRITE || mode == DAF_APPEND)
    {

        // plain dump the new block after the DAF header or last block
        fseek                                   (dst->file, 0,       SEEK_CUR                                           );
        EVIL_FWRITE                             (uchar,     objsize, src, dst                                           );
        fseek                                   (dst->file, 0,       SEEK_CUR                                           );

    }

    else if(mode == DAF_UPDATE)
    {
        // copy first half to tempfile
        uint blocksize = 0; if(offset > 0)      { blocksize = curdaf->offsets[offset];                                  }
        rewind                                  (dst->file                                                              );
        ZJC_copy_block                          (dst, tmpdst, blocksize                                                 );

        // write new block
        fseek                                   (dst->file, 0,       SEEK_CUR                                           );
        EVIL_FWRITE                             (uchar,     objsize, src, tmpdst                                        );
        fseek                                   (dst->file, 0,       SEEK_CUR                                           );
    }

    return 0;                                                                                                           }

/*

daf_update_end;

{
        // if not last block, copy remainder to tempfile
        if                                      (!(offset == (ZJC_DAFSIZE - 1) || offset == (curdaf->fileCount - 1)    ))

        {   uint leap = curdaf->offsets[offset+1] - blocksize; blocksize = curdaf->size - curdaf->offsets[offset+1];

            fseek                               (dst->file, 0,      SEEK_CUR                                            );
            fseek                               (dst->file, leap,   SEEK_CUR                                            );
            fseek                               (dst->file, 0,      SEEK_CUR                                            );

            ZJC_copy_block                      (dst,       tmpdst, blocksize                                           );
                                                                                                                        }
}
*/

//  - --- - --- - --- - --- -

int read_crkdump(cchar*  filename,
                 cchar*  archive,
                 uint8_t mode,
                 uint8_t offset    )            {

    int    evilstate    = 0;

    cchar* tmpsrc_name;
    cchar* tmpvrt_name;
    cchar* tmpdst_name;

    catpath                                     (filename, "TMP",    &tmpsrc_name                                       );
    catpath                                     (filename, "TMPVRT", &tmpvrt_name                                       );
    catpath                                     (archive,  "TMP",    &tmpdst_name                                       );

    WARD_EVIL_MALLOC                            (curdaf, DAF, sizeof(DAF), 1                                            );
    WARD_EVIL_MALLOC                            (curcrk, CRK, sizeof(CRK), 1                                            );

    ZBIN src            =                       { filename,    NULL                                                     };
    ZBIN dst            =                       { archive,     NULL                                                     };
    ZBIN tmpsrc         =                       { tmpsrc_name, NULL                                                     };
    ZBIN tmpvrt         =                       { tmpvrt_name, NULL                                                     };
    ZBIN tmpdst         =                       { tmpdst_name, NULL                                                     };

    WARD_EVIL_WRAP                              (evilstate, openbin(&src, "rb", 0)                                      );

    { cchar* readmode   = get_fmode             (mode                                                                   );
      WARD_EVIL_WRAP                            (evilstate, openarch(&dst, readmode, CRKSIGN, mode, 0));                }

    WARD_EVIL_WRAP                              (evilstate, openbin(&tmpsrc, "wb+", 1)                                  );
    WARD_EVIL_WRAP                              (evilstate, openbin(&tmpdst, "wb+", 1)                                  );

    EVIL_FREAD                                  (CRK, 1, curcrk, (&src)                                                 );

    if(curcrk->vertCount > CRK_MAXVERTS)        { printf("CRK vert count over max! (%u > %u)",
                                                         curcrk->vertCount, CRK_MAXVERTS     ); return 1;               }

    ZJC_new_dafblock                            (curcrk, &dst, &tmpdst, sizeof(curcrk), mode, offset                    );
    ZBIN* trvdst = &dst; if(mode == DAF_UPDATE) { trvdst = &tmpdst;                                                     }

//  - --- - --- - --- - --- -

    // read/write boxes

  { RWB3D* rwbuff    = NULL; WARD_EVIL_MALLOC   (rwbuff, RWB3D, sizeof(RWB3D), curcrk->boxCount                         );
                             EVIL_FREAD         (RWB3D, curcrk->boxCount, rwbuff, (&src)                                );

    BP3D   boxpacked = {0};
    for(uint i = 0; i < curcrk->boxCount; i++)  { ZJC_pack_rawbox(&boxpacked, rwbuff + i       );
                                                  EVIL_FWRITE    (BP3D, 1, (&boxpacked), trvdst);                       }

    WARD_EVIL_MFREE(rwbuff);                                                                                            }

    // copy tri idex to file as-is
    ZJC_copy_block                              (&src, trvdst, (curcrk->triCount * 3) * 2                               );

//  - --- - --- - --- - --- -

    // read/write verts

  { RWV3D* rwbuff     = NULL; WARD_EVIL_MALLOC  (rwbuff,     RWV3D, sizeof(RWV3D), curcrk->vertCount                    );
    VP3D*  vertpacked = NULL; WARD_EVIL_MALLOC  (vertpacked, VP3D,  sizeof(VP3D ), curcrk->vertCount                    );

    uint size_i = 0; uint size_d = 0;

//  - --- - --- - --- - --- -

    // entire mesh is read once per frame
    for(uint i = 0; i < curcrk->frameCount; i++)
    {
        float bindsmat[256];

        EVIL_FREAD (float, curcrk->numBinds * 16, bindsmat, (&src));
        EVIL_FREAD (RWV3D, curcrk->vertCount,     rwbuff,   (&src));

        EVIL_FWRITE(float, curcrk->numBinds * 16, bindsmat, trvdst);

        for(uint i = 0; i < curcrk->vertCount; i++) { ZJC_pack_rawvert(vertpacked + i, rwbuff + i);                     }
        EVIL_FWRITE                                 (VP3D, curcrk->vertCount, vertpacked, (&tmpsrc)                     );

        size_i += curcrk->vertCount * sizeof(VP3D);
    }

//  - --- - --- - --- - --- -

    // with every frame written, compress and write
    ZJC_DEFLATEFILE(&tmpsrc, &tmpvrt, curcrk->vertCount * sizeof(VP3D), curcrk->frameCount, size_i, &size_d);

    uint sizes[2] = { size_i, size_d }; EVIL_FWRITE(uint, 2, sizes, trvdst);
    ZJC_copy_block(&tmpvrt, trvdst, size_d);

    WARD_EVIL_MFREE(rwbuff); WARD_EVIL_MFREE(vertpacked);                                                               }

//  - --- - --- - --- - --- -

    // resolve DAF update

    WARD_EVIL_WRAP                              (evilstate, closebin(filename, 0)                                       );

//  - --- - --- - --- - --- -

    WARD_EVIL_WRAP(evilstate, remove(filename));
    printf("Deleted file <%s>\n", filename);

    return 0;                                                                                                           }

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

    curjoj_data = (uchar*) evil_malloc(joj->datasize_i, sizeof(uchar));
    DEFLDAF(imdata, curjoj_data, joj->datasize_i, &joj->datasize_d);

    WARD_EVIL_MFREE(imdata);                                                                                            }

void joj_subTexRead (uint    dim,
                     uchar   imcount,
                     uchar   curim,
                     float** pixels   )         {

    for(uint i = (dim * curim), j = 0; i < (dim * (curim + 1)); i++, j += 4)
    {
    /*

        H8PACK has fallen from grace. migrate this disgusting block to zlib

        JOJPIX pix = { hpckdata[i + (dim * imcount      )],     // chroma_u
                       hpckdata[i + ((dim * 2) * imcount)],     // chroma_v
                       hpckdata[i                        ],     // luma
                       hpckdata[i + ((dim * 3) * imcount)]  };  // alpha

        joj_to_rgba( (*pixels)+j, &pix );
    */
    }
                                                                                                                        }

//  - --- - --- - --- - --- -

int read_jojdump(cchar*   filename,
                 JOJ*     joj,
                 JOJMATE* mate,
                 uchar*   mateoff   )           {
/*
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

    joj->datasize_d    =  0;

    joj_texPack(joj, dim, pixels);

    curjoj_size        = sizeof(JOJ) + sizeof(joj->datasize_d);

    WARD_EVIL_MFREE(pixels);
    WARD_EVIL_WRAP(errorstate, remove(filename));

    printf("Deleted file <%s>\n", filename);
*/
    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int crk_to_daf(CRK*   crk,
               cchar* filename)                 {
/*
    fseek(curfile, 0, SEEK_CUR);

    EVIL_FWRITE(CRK,     1,                   crk,                 filename);
    EVIL_FWRITE(pVP3D_8, 8,                   curcrk_data.bounds,  filename);
    EVIL_FWRITE(VP3D_8,  crk->vertCount,      curcrk_data.verts,   filename);
    EVIL_FWRITE(ushort,  crk->indexCount * 3, curcrk_data.indices, filename);

    fseek(curfile, 0, SEEK_CUR);
*/
    return 0;                                                                                                           }

int joj_to_daf(JOJ*     joj,
               cchar*   filename)               {
/*
    fseek      (curfile, 0,               SEEK_CUR               );

    EVIL_FWRITE(JOJ,     1,               joj,           filename);
    EVIL_FWRITE(uchar,   joj->datasize_d, curjoj_data,   filename);

    fseek      (curfile, 0,               SEEK_CUR               );
*/
    return 0;                                                                                                           }

int jojmate_to_daf(JOJMATE* mate,
                   cchar*   filename)           {
/*
    float  fvalues[4] = { mate->spec_mult, mate->diff_mult, mate->ref_mult, mate->glow_rea };
    ushort svalues[2] = { mate->shdid,     mate->flags                                     };

    fseek               (curfile, 0,       SEEK_CUR                                        );

    EVIL_FWRITE         (float,   4,       fvalues, filename                               );
    EVIL_FWRITE         (ushort,  2,       svalues, filename                               );

    fseek               (curfile, 0,       SEEK_CUR                                        );

    printf(" %f, %f, %f, %f | %u, %u | FUK\n",

           fvalues[0], fvalues[1], fvalues[2], fvalues[3],
           svalues[0], svalues[1]                           );
*/
    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int writedaf(void*      obj,
             cchar      sign[8],
             uint       objsize,

             JOJMATE*   mate,
             uchar      mateoff,

             uint8_t    mode,
             uint8_t    offset,
             cchar*     filename)           {

    int evilstate = 0;
    int isJOJ     = !strcmp(sign, JOJSIGN);

    {
        cchar* readmode = get_fmode(mode);
        WARD_EVIL_WRAP(evilstate, openarch(filename,
                                           readmode,
                                           sign,
                                           mode,
                                           0        ));
    }

    if      (curdaf->fileCount == ZJC_DAFSIZE
            && mode != DAF_UPDATE)              { printf("Archive <%s> is full; addition aborted.\n", filename);
                                                  return 0;                                                             }

    if      (mode == DAF_WRITE)
    {
        if  (isJOJ)                             { WARD_EVIL_WRAP(evilstate, joj_to_daf(obj, filename)); }
        else                                    { WARD_EVIL_WRAP(evilstate, crk_to_daf(obj, filename)); }

        curdaf->fileCount++;}
/*
        rewind(curfile             );
        fseek (curfile, 8, SEEK_CUR);
        fseek (curfile, 0, SEEK_CUR);

        uint32_t newsize_offset[2] = { curdaf->size + objsize, sizeof(DAF) + (JDAF_MATEBLOCK * (mate != NULL)) };

        EVIL_FWRITE(ushort,   1, &curdaf->fileCount, filename);
        EVIL_FWRITE(uint32_t, 2, newsize_offset,  filename);

    }

    else if (mode == DAF_APPEND)
    {
        if  (isJOJ)                             { WARD_EVIL_WRAP(evilstate, joj_to_daf(obj, filename)); }
        else                                    { WARD_EVIL_WRAP(evilstate, crk_to_daf(obj, filename)); }

        closebin(filename, 1);
        openbin (filename, "rb+", 1);

        fseek (curfile, 8, SEEK_CUR);
        fseek (curfile, 0, SEEK_CUR);

        uint32_t newsize   = curdaf->size + objsize;
        uint32_t newoffset = curdaf->size;

        uint16_t offset_stride = (curdaf->fileCount) * 4;

        curdaf->fileCount++;
        EVIL_FWRITE(ushort,   1, &curdaf->fileCount,  filename);
        EVIL_FWRITE(uint32_t, 1, &newsize,         filename);

        fseek (curfile, 0,             SEEK_CUR);
        fseek (curfile, offset_stride, SEEK_CUR);

        EVIL_FWRITE(uint32_t, 1, &newoffset,       filename);
    }

    else if (mode == DAF_UPDATE && offset < ZJC_DAFSIZE)
    {

        int isLastChunk = offset == (ZJC_DAFSIZE - 1) || offset == (curdaf->fileCount - 1);

        uint32_t chunk_start = curdaf->offsets[offset];
        uint32_t newsize, byteshift, old_end;

        if(isLastChunk)
        {
            newsize   = chunk_start + objsize;
            old_end   = curdaf->size;
            byteshift = newsize - old_end;

            fseek (curfile, 0,           SEEK_CUR);
            fseek (curfile, chunk_start, SEEK_CUR);

            if  (isJOJ)                         { WARD_EVIL_WRAP(evilstate, joj_to_daf(obj, filename)); }
            else                                { WARD_EVIL_WRAP(evilstate, crk_to_daf(obj, filename)); }

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

            chunk_end = chunk_start + objsize;
            old_end   = curdaf->offsets[offset+1];
            byteshift = chunk_end - old_end;
            newsize   = (curdaf->size - (old_end - chunk_start)) + objsize;

            fseek(curfile, 0,           SEEK_CUR);
            fseek(curfile, chunk_start, SEEK_CUR);

            if  (isJOJ)                         { WARD_EVIL_WRAP(evilstate, joj_to_daf(obj, filename)); }
            else                                { WARD_EVIL_WRAP(evilstate, crk_to_daf(obj, filename)); }

            curdaf_data += (old_end - sizeof(DAF));
            fseek(curfile, 0,         SEEK_CUR);
            fseek(curfile, chunk_end, SEEK_CUR);
            fseek(curfile, 0,         SEEK_CUR);
            EVIL_FWRITE(uchar, curdaf->size - old_end, curdaf_data, filename);
            fseek(curfile, 0,         SEEK_CUR);

            for(uint i = offset+1;
                i < curdaf->fileCount; i++)        { curdaf->offsets[i] += byteshift;                                         }

            rewind(curfile);
            fseek(curfile, 10, SEEK_CUR);
            fseek(curfile, 0, SEEK_CUR);
            EVIL_FWRITE(uint32_t, 1,   &newsize,     filename);
            EVIL_FWRITE(uint32_t, ZJC_DAFSIZE, curdaf->offsets, filename);
            fseek(curfile, 0, SEEK_CUR);
        }

        _chsize_s(_fileno(curfile), curdaf->size + byteshift);

    }

    if(mate)
    {
        if(!(mate->flags & 128))                { uint first_stride = sizeof(JOJMATE) * mateoff;
                                                  uint final_stride = JDAF_MATEBLOCK - (first_stride + sizeof(JOJMATE)  );

                                                  rewind        (curfile                                                );
                                                  fseek         (curfile,   sizeof(DAF),  SEEK_CUR                      );
                                                  fseek         (curfile,   0,            SEEK_CUR                      );

                                                  fseek         (curfile,   0,            SEEK_CUR                      );
                                                  fseek         (curfile,   first_stride, SEEK_CUR                      );

                                                  WARD_EVIL_WRAP(evilstate, jojmate_to_daf(mate, filename)              );

                                                  fseek         (curfile,   final_stride, SEEK_CUR                      );
                                                  fseek         (curfile,   0,            SEEK_CUR                      );

                                                                                                                        }
    }

    closebin(filename, 0);*/
    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int readssx(SSX* ssx, cchar* filename)          {

    int evilstate = 0;
    WARD_EVIL_WRAP(evilstate, openbin(filename, "rb", 0));
/*
    char sign[8];
    fread(sign, sizeof(cchar), 8, curfile);

    GETLOC;
    for(uint i = 0; i < 8; i++)                 { if(sign[i] != SSXSIGN[i])
                                                { __terminator__(68, bin->name); return ERROR; }                             }

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

    WARD_EVIL_WRAP(evilstate, closebin(filename, 0));*/
    return 0;                                                                                                           }
//  - --- - --- - --- - --- -

int popirf(DAF*    daf,
           uint8_t offset,
           cchar*  filename)                {
/*
    int evilstate = 0;

    {
        WARD_EVIL_WRAP(evilstate, openarch(filename,
                                           "rb+",
                                           CRKSIGN,
                                           4,
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

            curdaf_data += (old_end - sizeof(DAF));
            fseek(curfile, 0,           SEEK_CUR);
            fseek(curfile, chunk_start, SEEK_CUR);
            fseek(curfile, 0,           SEEK_CUR);
            EVIL_FWRITE(uchar, daf->size - old_end, curdaf_data, filename);
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
*/
    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int writecrk(cchar* filename,
             cchar* archive,
             char*  mode,
             char*  offset)                     {

    int    evilstate  = 0;

    uint8_t numoffset = (uint8_t) hexstr_tolong (offset                                                                 );
    uint8_t nummode   = (uint8_t) hexstr_tolong (mode                                                                   );

    zjc_convertor_init                          (BUILD_FRAC                                                             );
    evilstate = read_crkdump                    (filename, archive, numoffset, nummode                                  );
    zjc_convertor_end                           (                                                                       );

    /*if(!evilstate)                              { evilstate = writedaf(curcrk, CRKSIGN, curcrk_size, NULL,
                                                                       0, nummode, numoffset, archive      );           }

    if(nummode == DAF_UPDATE || DAF_DELETE)     { WARD_EVIL_MFREE(curdaf_data);                                         }*/

    ZJC_CRK_END                                 (                                                                       );

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
/*
    if(!evilstate)                              { evilstate = writedaf(&joj, JOJSIGN, curjoj_size, &mate,
                                                                       mateoff, nummode, numoffset, archive);           }
*/
    if(nummode == DAF_UPDATE || DAF_DELETE)     { WARD_EVIL_MFREE(curdaf_data);                                         }

    del_JojFile                                 (&joj                                                                   );

    return evilstate;                                                                                                   }

//  - --- - --- - --- - --- -

cchar* get_archtype(uint8_t archtype)           {

    if     (archtype == 0)                      { return CRKSIGN;                                                       }
    else if(archtype == 1)                      { return JOJSIGN;                                                       }
    else                                        { return SSXSIGN;                                                       }
                                                                                                                        }

int    extraction_start (cchar*  filename,
                         uchar   archtype  )    {

    int evilstate   = 0;
    cchar* sign     = get_archtype(archtype);
    cchar* readmode = get_fmode(DAF_READ);

    WARD_EVIL_WRAP(evilstate, openarch(filename,
                                       readmode,
                                       sign,
                                       DAF_READ,
                                       0        ));

    return 0;                                                                                                           }

int    extraction_end   (cchar* filename)       {
/*
    if(curhpck.dictsize) { WARD_EVIL_MFREE(hpckdata); zh8_delPacker(&curhpck); }

    closebin(filename, 0);
    memset(curdaf, 0, sizeof(*curdaf));
*/
    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int extractcrk (uchar offset, uchar start)      {

    //  move pointer to first block (@offset) on first read
    if(!start)                                  {
/*
        if(!curdaf)                             { printf("No DAF currently open. Cannot extract.\n"); return 1;         }

        rewind                                  (curfile                                                                );

        fseek                                   (curfile, 0,                       SEEK_CUR                             );
        fseek                                   (curfile, curdaf->offsets[offset], SEEK_CUR                             );
        fseek                                   (curfile, 0,                       SEEK_CUR                             );
                                                                                                                        }
*/
//  - --- - --- - --- - --- -
/*
    // read next item
    fread                                       (curcrk,              sizeof(CRK    ), 1,                   curfile     );

    if     (curcrk->vertCount  > MAXVERTS)      { printf("CRK vert count over max! (%u > %u)",
                                                         curcrk->vertCount, MAXVERTS); return 1;                        }

    else if(curcrk->indexCount > MAXVERTS)      { printf("CRK index count over max! (%u > %u)",
                                                         curcrk->indexCount, MAXVERTS); return 1;                       }
*/
/*
    fread                                       (curcrk_data.bounds,  sizeof(pVP3D_8), 8,                   curfile     );
    fread                                       (curcrk_data.verts,   sizeof(VP3D_8 ), curcrk->vertCount,   curfile     );
    fread                                       (curcrk_data.indices, sizeof(ushort ), curcrk->indexCount,  curfile     );
*/
    return 0;                                                                                                           }

//  - --- - --- - --- - --- -
/*
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

    uint   ivalues[2] = { 0, 0 };
    ushort svalues[2] = { 0, 0 };

    fseek      (curfile, 0,               SEEK_CUR               );

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
*/
