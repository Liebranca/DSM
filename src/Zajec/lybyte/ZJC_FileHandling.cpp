#include <cstdio>
#include <cstdint>

#include "ZJC_FileHandling.h"
#include "ZJC_BinTypes.h"

#include "../lyutils/ZJC_Evil.h"

//  - --- - --- - --- - --- -

#define EVIL_FREAD(type, count, buff)             buff = (type*) zjc::evil_malloc(count, sizeof(type));\
                                                  fread(buff, sizeof(type), count, zjc::curfile);

#define EVIL_FWRITE(type, count, buff, info)      WARD_EVIL_FWRITE(fwrite(buff, sizeof(type),\
                                                  count, curfile), info, count);

//  - --- - --- - --- - --- -

extern "C" namespace zjc {

    static    FILE*  curfile;
    static    FILE*  tempfile;

    constexpr ushort maxstride    =               0xFFFF;

    cchar crk_sign[16]              =           { 0x46, 0x43, 0x4b, 0x42, 0x21, 0x54, 0x43, 0x48,
                                                  0x45, 0x53, 0x47, 0x45, 0x54, 0x24, 0x24, 0x24                        };

//  - --- - --- - --- - --- -

    struct CrkFile                              {

        CrkFile()                               {                                                                       }
        ~CrkFile()                              { WARD_EVIL_MFREE(bounds); WARD_EVIL_MFREE(verts);
                                                  WARD_EVIL_MFREE(indices);                                             }

        ushort vertCount;
        ushort indexCount;

        PhysVertexPacked3D* bounds;
        VertexPacked3D* verts;
        ushort* indices;

                                                                                                                        };

//  - --- - --- - --- - --- -

    struct IrfArchive                           {

        ushort fileCount;
        uint size;
        ushort offsets[512];

        ushort_pair operator [] (ushort i)      { if( i > 255 ) { return {0, 0}; }
                                                  return { offsets[i*2], offsets[(i*2)+1] }; }
};

//  - --- - --- - --- - --- -

    int openbin(cchar* filename,
                cchar* mode,
                bool shutit)                    {

        int isnew = 0;
        curfile = fopen(filename, mode);

        if( (mode == "rb+")
         && (curfile == NULL) )                 { curfile = fopen(filename, "wb"); isnew = -1;                          }
                                                  WARD_EVIL_FOPEN(curfile,  filename);

        if (!shutit)                            { printf("Opened file <%s>\n", filename);                               }

        return isnew;                                                                                                   }

//  - --- - --- - --- - --- -

    int readarch(IrfArchive irf,
                 uint start,
                 uint end)                      {

        ;
    }

//  - --- - --- - --- - --- -

    int closebin(cchar* filename,
                  bool shutit)                  {

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

        WARD_EVIL_WRAP(errorstate, openbin(filename, "rb", false));

        fread(sizes, sizeof(ushort), 2, zjc::curfile);

        crk->vertCount = sizes[0];
        crk->indexCount = sizes[1];

        EVIL_FREAD(float, 24, bounds);
        EVIL_FREAD(float, crk->vertCount * 8, verts);
        EVIL_FREAD(ushort, crk->indexCount * 3, crk->indices);

        WARD_EVIL_WRAP(errorstate, closebin(filename, false));

//  - --- - --- - --- - --- -

        crk->bounds = (PhysVertexPacked3D*) evil_malloc(8, sizeof(PhysVertexPacked3D));
        crk->verts = (VertexPacked3D*) evil_malloc(*sizes, sizeof(VertexPacked3D));

        for(ushort i = 0, j = 0;
            i < crk->vertCount * 8;
            i+= 8, j++ )                        { *(crk->verts+(j)) = (verts+i);                                        }

        for(ushort i = 0, j = 0;
            i < 24; i+= 3, j++ )                { *(crk->bounds+(j)) = (bounds+i);                                      }

        uint filesize = 4 + (8*6) + (crk->vertCount * 16) + (crk->indexCount * 2);

//  - --- - --- - --- - --- -

        {   int isnew = openbin(archive, "rb+", false);
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
        
        WARD_EVIL_WRAP(errorstate, closebin(archive, false));
        WARD_EVIL_WRAP(errorstate, remove(filename));
        printf("Deleted file <%s>\n", filename);

        return 0;
                                                                                                                        }

//  - --- - --- - --- - --- -

    int writecrk(cchar* filename,
                 cchar* archive,
                 char* offset,
                 char* mode)                 {

        CrkFile* crk = new CrkFile;

        ushort sizes[2] = {0, 0};
        float* bounds = nullptr;
        float* verts = nullptr;

        uint8_t numoffset = HEXLIT(offset, uint8_t);
        uint8_t nummode   = HEXLIT(mode,   uint8_t);

        int evilstate = IN_writecrk(filename, archive, numoffset, nummode,
                                    crk, sizes, bounds, verts);

        delete crk;

        WARD_EVIL_MFREE(bounds);
        WARD_EVIL_MFREE(verts);

        return evilstate;                                                                                               }

//  - --- - --- - --- - --- -

}