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
                                                                count, bin->name);

#define EVIL_FREAD2(readcount, type, count, buff, bin) readcount = fread(buff, sizeof(type), count, bin->file );              \
                                                  WARD_EVIL_FREAD  (readcount, count, bin->name          );

#define EVIL_FWRITE(type, count, buff, bin)     WARD_EVIL_FWRITE(fwrite(buff, sizeof(type), count, bin->file),           \
                                                                 count, bin->name                           );

//  - --- - --- - --- - --- -

#define CHUNK 0x4000

#define CALL_ZLIB(x) {                                                      \
        int status;                                                         \
        status = x;                                                         \
        if (status < 0) {                                                   \
            fprintf       (stderr,                                          \
                           "%s:%d: %s returned a bad status of %d.\n",      \
                           __FILE__, __LINE__, #x, status);                 \
            __terminator__(0, "Bruh, zLib went PLOP!");                     \
        }                                                                   \
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

int ZJC_INFLATEFILE(ZBIN* src,
                    ZBIN* dst,

                    uint size_i,
                    uint size_d)                {

    z_stream strm = {0};

    uchar    in [CHUNK];
    uchar    out[CHUNK];

    uint     readsize = CHUNK;
    uint     dataleft = size_i;

    strm.zalloc       = Z_NULL;
    strm.zfree        = Z_NULL;
    strm.opaque       = Z_NULL;

    strm.next_in      = in;
    strm.avail_in     = 0;

    CALL_ZLIB(inflateInit2(&strm, 15 | 32));

    while(dataleft)
    {
        int  bytes_read;
        int  zlib_status;

        if(readsize > dataleft)                 { readsize = dataleft;                                                  }
        EVIL_FREAD2                             (bytes_read, uchar, readsize, in, src                                   );

        dataleft      -= readsize;
        strm.avail_in  = bytes_read;
        strm.next_in   = in;

        while(strm.avail_out == 0)
        {
            uint have;

            strm.avail_out = CHUNK;
            strm.next_out  = out;
            zlib_status    = inflate            (&strm, Z_NO_FLUSH                                                      );

            switch (zlib_status)                {   case Z_OK:
                                                    case Z_STREAM_END:
                                                    case Z_BUF_ERROR:
                                                    break;

                                                    default:
                                                    inflateEnd (&strm);
                                                    fprintf    (stderr, "Gzip error %d in '%s'.\n",
                                                                        zlib_status, src->name    );

                                                    return ERROR;                                                       }

            have = CHUNK - strm.avail_out;

            EVIL_FWRITE                         (uchar, have, out, dst                                                  );

        }
    }

    return 0;                                                                                                           }

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
    uchar    out[CHUNK];
    uchar*   readbuff  = NULL; WARD_EVIL_MALLOC(readbuff, uchar, blocksize, 1);

    z_stream strm;

    defstrm_init(&strm);
    rewind(src->file);

    for(uint i = 0; i < numblocks; i++)
    {
        // read next block to compress
        fseek                                   (src->file, 0, SEEK_CUR                                                 );
        EVIL_FREAD                              (uchar, blocksize, readbuff, src                                        );
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

            EVIL_FWRITE(uchar, have, out, dst);

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
static cuint           CRK_MAXBOXES    =          0x10;
static cuint           CRK_MAXSTATES   =          0x20;
static cuint           CRK_MAXBINDS    =          0x10 * 0x20;

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
static MD3D*   curcrk_data    = NULL;

static JOJ*    curjoj         = NULL;

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

void del_SsxFile(SSX* ssx)                      {

    WARD_EVIL_MFREE(ssx->objects);                                                                                      }

//  - --- - --- - --- - --- -

int openarch(ZBIN*         bin,
             cchar*        readmode,
             cchar         archtype[8],

             const uint8_t mode,
             int           shutit)              {

    int isnew         = 0;
    int errorstate    = 0;

    isnew             =  openbin                (bin,      readmode, shutit                                             );

//  - --- - --- - --- - --- -

    if(isnew)
    {
        strcpy                                  (curdaf->sign,    archtype                                              );

        curdaf->fileCount                       = 0;
        curdaf->size                            = sizeof(DAF);

        memset                                  (curdaf->offsets, 0,           sizeof(curdaf->offsets)                  );

        EVIL_FWRITE                             (DAF,             1,           curdaf, bin                              );
    }

//  - --- - --- - --- - --- -

    else
    {

        if(isnew && mode == DAF_READ     )      { fprintf(stderr, "Archive %s does not exist.\n", bin->name);
                                                  WARD_EVIL_WRAP(errorstate, remove(bin->name)); return ERROR;          }

        rewind                                  (bin->file                                                              );

        EVIL_FREAD                              (DAF, 1, curdaf, bin                                                    );

        if(strcmp(curdaf->sign, archtype))      { __terminator__(68, bin->name); return ERROR;                          }

    }

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int ZJC_new_dafblock(void*   src,
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
        uint blocksize = sizeof(DAF);
        if(offset > 0)                          { blocksize = curdaf->offsets[offset];                                  }

        rewind                                  (dst->file                                                              );
        ZJC_copy_block                          (dst, tmpdst, blocksize                                                 );

        // write new block
        fseek                                   (tmpdst->file, 0,       SEEK_CUR                                        );
        EVIL_FWRITE                             (uchar,     objsize, src, tmpdst                                        );
        fseek                                   (tmpdst->file, 0,       SEEK_CUR                                        );
    }

    return 0;                                                                                                           }

int ZJC_end_dafblock(ZBIN*   dst,
                     ZBIN*   tmpdst,

                     uint    blocksize,

                     uint8_t mode,
                     uint8_t offset)            {

    int evilstate = 0;

    if(curdaf->fileCount >= ZJC_DAFSIZE
      && mode != DAF_UPDATE)                    { printf("Archive <%s> is full; addition aborted.\n", dst->name);
                                                  return 0;                                                             }

    if(mode == DAF_WRITE || mode == DAF_APPEND)
    {

        if(mode == DAF_APPEND)                  { closebin(dst, 1); openbin(dst, "rb+", 1); /* <- sneaky re-open*/      }

        curdaf->offsets[curdaf->fileCount] = curdaf->size;
        curdaf->fileCount++;
        curdaf->size += blocksize;

        rewind                                  (dst->file                                                              );
        EVIL_FWRITE                             (DAF, 1, curdaf, dst                                                    );

    }

//  - --- - --- - --- - --- -

    else if(mode == DAF_UPDATE)                 {

        uint  chunk_start = curdaf->offsets[offset];

        // if @offset is not the last block in DAF, then copy the remainder
        if                                      (!(offset == (ZJC_DAFSIZE - 1) || offset == (curdaf->fileCount - 1)    ))

        {   uint leap = curdaf->offsets[offset+1] - blocksize; blocksize = curdaf->size - curdaf->offsets[offset+1];

            fseek                               (dst->file, 0,      SEEK_CUR                                            );
            fseek                               (dst->file, leap,   SEEK_CUR                                            );
            fseek                               (dst->file, 0,      SEEK_CUR                                            );

            ZJC_copy_block                      (dst,       tmpdst, blocksize                                           );

            uint newsize;
            uint old_end;
            uint chunk_end;
            uint byteshift;

            chunk_end   =                       chunk_start + blocksize;
            old_end     =                       curdaf->offsets[offset+1];
            byteshift   =                       chunk_end - old_end;
            newsize     =                       (curdaf->size - (old_end - chunk_start)) + blocksize;

            // adjust the start of block markers for subsequent blocks in DAF to reflect the new size
            for                                 (uint i = offset+1; i < curdaf->fileCount; i++                          )
                                                { curdaf->offsets[i] += byteshift;                                      }

            curdaf->size = newsize;                                                                                     }

//  - --- - --- - --- - --- -

        // no adjustment needed when @offset IS the last block; just assign new size in this case
        else                                    { curdaf->size = chunk_start + blocksize;                               }

        // write updated DAF
        rewind                                  (tmpdst->file                                                           );
        fseek                                   (tmpdst->file, 0,      SEEK_CUR                                         );
        EVIL_FWRITE                             (DAF, 1, curdaf, tmpdst                                                 );
        fseek                                   (tmpdst->file, 0,      SEEK_CUR                                         );

        // now close the files, delete the original & rename tmp to take its place
        WARD_EVIL_WRAP                          (evilstate, closebin(dst,    1) /* <- sneaky fclose*/                   );
        WARD_EVIL_WRAP                          (evilstate, closebin(tmpdst, 1)                                         );

        WARD_EVIL_WRAP                          (evilstate, remove(dst->name              )                             );
        WARD_EVIL_WRAP                          (evilstate, rename(tmpdst->name, dst->name)                             );

                                                                                                                        }
    return 0;                                                                                                           }


//  - --- - --- - --- - --- -

int read_crkdump(cchar*  filename,
                 cchar*  archive,
                 uint8_t mode,
                 uint8_t offset    )            {

    int    evilstate    = 0;

    char* tmpsrc_name;
    char* tmpvrt_name;
    char* tmpdst_name;

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

    uint blocksize      =                       sizeof(CRK);
    WARD_EVIL_WRAP                              (evilstate, openbin(&src, "rb", 0)                                      );

    { cchar* readmode   = get_fmode             (mode                                                                   );
      WARD_EVIL_WRAP                            (evilstate, openarch(&dst, readmode, CRKSIGN, mode, 0));                }

    openbin                                     (&tmpsrc, "wb+", 1                                                      );
    openbin                                     (&tmpvrt, "wb+", 1                                                      );
    openbin                                     (&tmpdst, "wb+", 1                                                      );

    EVIL_FREAD                                  (CRK, 1, curcrk, (&src)                                                 );

    if(curcrk->vertCount > CRK_MAXVERTS)        { printf("CRK vert count over max! (%u > %u)",
                                                         curcrk->vertCount, CRK_MAXVERTS     ); return 1;               }

    ZJC_new_dafblock                            (curcrk, &dst, &tmpdst, sizeof(*curcrk), mode, offset                   );
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

    blocksize +=                                (curcrk->boxCount * sizeof(BP3D) + (curcrk->triCount * 3 * 2)           );

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

    // with every frame written, deflate and write
    ZJC_DEFLATEFILE(&tmpsrc, &tmpvrt, curcrk->vertCount * sizeof(VP3D), curcrk->frameCount, size_i, &size_d);

    uint sizes[2] = { size_i, size_d }; EVIL_FWRITE(uint, 2, sizes, trvdst);
    rewind(tmpvrt.file);
    ZJC_copy_block(&tmpvrt, trvdst, size_d);

    blocksize    += size_d + sizeof(sizes) + (curcrk->numBinds * 16 * curcrk->frameCount * sizeof(float));

    WARD_EVIL_MFREE(rwbuff); WARD_EVIL_MFREE(vertpacked);                                                               }

//  - --- - --- - --- - --- -

    // resolve DAF update
    ZJC_end_dafblock                            (&dst, &tmpdst, blocksize, mode, offset                                 );

    // now clean-up your mess
    WARD_EVIL_WRAP                              (evilstate, closebin(&src, 0 )                                          );
    WARD_EVIL_WRAP                              (evilstate, remove  (src.name)                                          );
    printf                                      ("Deleted file <%s>\n", src.name                                        );

    if(mode != DAF_UPDATE)
    { WARD_EVIL_WRAP                            (evilstate, closebin(&dst,    0 )                                       );
      WARD_EVIL_WRAP                            (evilstate, closebin(&tmpdst, 1 )                                       );
      WARD_EVIL_WRAP                            (evilstate, remove  (tmpdst.name)                                       );
                                                                                                                        }

    WARD_EVIL_WRAP                              (evilstate, closebin(&tmpsrc, 1 )                                       );
    WARD_EVIL_WRAP                              (evilstate, closebin(&tmpvrt, 1 )                                       );
    WARD_EVIL_WRAP                              (evilstate, remove  (tmpsrc.name)                                       );
    WARD_EVIL_WRAP                              (evilstate, remove  (tmpvrt.name)                                       );

    WARD_EVIL_MFREE                             (tmpsrc_name                                                            );
    WARD_EVIL_MFREE                             (tmpdst_name                                                            );
    WARD_EVIL_MFREE                             (tmpvrt_name                                                            );

    WARD_EVIL_MFREE                             (curdaf                                                                 );
    WARD_EVIL_MFREE                             (curcrk                                                                 );

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

void joj_texPack   (JOJ*   joj,
                    uint   dim,
                    float* pixels)              {

    /*JOJPIX curcol    = { 0 };
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

    WARD_EVIL_MFREE(imdata);*/                                                                                            }

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

int readssx(SSX* ssx, cchar* filename)          {

    char sign[8];

    int  evilstate = 0; 
    ZBIN src       = { filename, NULL };

    WARD_EVIL_WRAP                              (evilstate, openbin(&src, "rb", 0)                                      );


    EVIL_FREAD                                  (cchar, 8, sign, (&src)                                                 );
    WARD_EVIL_FSIGN                             (sign, SSXSIGN, src.name                                                );

    EVIL_FREAD                                  (ushort, 1, &ssx->count, (&src)                                         );

    WARD_EVIL_MALLOC                            (ssx->objects, SSO, sizeof(SSO), ssx->count                             );

    for(ushort i = 0; i < ssx->count; i++)
    {

        EVIL_FREAD                              (ushort, 3, &ssx->objects[i].resinfo, (&src)                            );

        ushort flags  = ssx->objects[i].resinfo[2];
        uchar  fcount = 10;

        if(flags & 0x0004)                      { fcount += 12;                                                         }

        EVIL_FREAD                              (float, fcount, ssx->objects[i].fvalues, (&src)                         );

    }

    WARD_EVIL_WRAP(evilstate, closebin(&src, 0));
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

    uint8_t numoffset = (uint8_t) hexstr_tolong (offset                                                                 );
    uint8_t nummode   = (uint8_t) hexstr_tolong (mode                                                                   );

    return read_crkdump(filename, archive, numoffset, nummode);                                                         }

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

    evilstate = read_jojdump                    (filename, &joj, &mate, &mateoff                                        );

    return evilstate;                                                                                                   }

//  - --- - --- - --- - --- -

cchar* get_archtype(uint8_t archtype)           {

    if     (archtype == 0)                      { return CRKSIGN;                                                       }
    else if(archtype == 1)                      { return JOJSIGN;                                                       }
    else                                        { return SSXSIGN;                                                       }
                                                                                                                        }

int ZJC_open_daf(ZBIN*  src,
                 uchar  archtype  )             {

    int evilstate   = 0;

    cchar* sign     = get_archtype(archtype);
    cchar* readmode = get_fmode(DAF_READ);

    if(!curdaf)                                 { WARD_EVIL_MALLOC(curdaf, DAF, sizeof(DAF), 1);                        }

    WARD_EVIL_WRAP                              (evilstate, openarch(src, readmode, sign, DAF_READ, 0)                  );

    return 0;                                                                                                           }

int ZJC_close_daf(ZBIN* src, int isLast)        { closebin(src, 0); if(isLast) { WARD_EVIL_MFREE(curdaf); } return 0;   }

//  - --- - --- - --- - --- -

void ZJC_init_crk()                             {

    WARD_EVIL_MALLOC                            (curcrk,                CRK,    sizeof(CRK   ), 1                       );
    WARD_EVIL_MALLOC                            (curcrk_data,           MD3D,   sizeof(MD3D  ), 1                       );
    WARD_EVIL_MALLOC                            (curcrk_data->boxes,    BP3D,   sizeof(BP3D  ), CRK_MAXBOXES            );
    WARD_EVIL_MALLOC                            (curcrk_data->indices,  ushort, sizeof(ushort), CRK_MAXVERTS            );
    WARD_EVIL_MALLOC                            (curcrk_data->bindsmat, float,  sizeof(float ), CRK_MAXBINDS            );
    WARD_EVIL_MALLOC                            (curcrk_data->verts,    VP3D,   sizeof(VP3D  ), CRK_MAXVERTS            );
                                                                                                                        }

void ZJC_end_crk()                              {

    WARD_EVIL_MFREE                             (curcrk_data->verts                                                     );
    WARD_EVIL_MFREE                             (curcrk_data->bindsmat                                                  );
    WARD_EVIL_MFREE                             (curcrk_data->indices                                                   );
    WARD_EVIL_MFREE                             (curcrk_data->boxes                                                     );
    WARD_EVIL_MFREE                             (curcrk_data                                                            );
    WARD_EVIL_MFREE                             (curcrk                                                                 );
                                                                                                                        }

MD3D*  ZJC_get_crkdata()                        { return curcrk_data;                                                   }
CRK*   ZJC_get_curcrk ()                        { return curcrk;                                                        }

//  - --- - --- - --- - --- -

int ZJC_extract_crk(ZBIN* src,
                    ZBIN* storage,

                    uchar offset,
                    uchar start )               {

    // move pointer to @offset on first read
    if(start)                                   {

        if(!curdaf)                             { printf("No DAF currently open. Cannot extract.\n"); return ERROR;     }

        rewind                                  (src->file                                                              );

        fseek                                   (src->file, 0,                       SEEK_CUR                           );
        fseek                                   (src->file, curdaf->offsets[offset], SEEK_CUR                           );
        fseek                                   (src->file, 0,                       SEEK_CUR                           );

        // open a new cache tempfile if storage isn't already open
        if(!storage->file)                      { int evilstate; WARD_EVIL_WRAP(evilstate, openbin(storage, "wb+", 1)); }
                                                                                                                        }
//  - --- - --- - --- - --- -

    // read next item
    EVIL_FREAD                                  (CRK, 1, curcrk, src                                                    );

    // should never happen. check just in case of corrupted files
    if(curcrk->vertCount  > CRK_MAXVERTS)       { printf("CRK vert count over max! (%u > %u)",
                                                         curcrk->vertCount, CRK_MAXVERTS); return ERROR;                }

    // read the uncompressed data
    EVIL_FREAD                                  (BP3D, curcrk->boxCount, curcrk_data->boxes, src                        );
    EVIL_FREAD                                  (ushort, curcrk->triCount * 3, curcrk_data->indices, src                );

    EVIL_FREAD                                  (float, curcrk->numBinds * 16 * curcrk->frameCount,
                                                 curcrk_data->bindsmat, src                                             );

    // get the blocksize and decompress verts
    uint sizes[2] = { 0, 0 }; EVIL_FREAD        (uint, 2, sizes, src                                                    );

    rewind                                      (storage->file                                                          );
    ZJC_INFLATEFILE                             (src, storage, sizes[0], sizes[1]                                       );
    rewind                                      (storage->file                                                          );

    return 0;                                                                                                           }

int ZJC_read_crkframe(ZBIN* storage)            { EVIL_FREAD(VP3D, curcrk->vertCount, curcrk_data, storage); return 0;  }

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
