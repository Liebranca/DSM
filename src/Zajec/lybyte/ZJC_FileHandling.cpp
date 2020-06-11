#include <iostream>
#include <stdio.h>

#include "ZJC_FileHandling.h"
#include "types.h"

#include "../lyutils/ZJC_Evil.h"

//  - --- - --- - --- - --- -

#define EVIL_FREAD(type, count, loc, buff)        buff = (type*) zjc::evil_malloc(count, sizeof(type), loc);\
                                                  fread(buff, sizeof(type), count, zjc::curfile);

#define EVIL_FWRITE(type, count, loc, buff, info) WARD_EVIL_FWRITE(fwrite(buff, sizeof(type),\
                                                  count, curfile), loc, info, count);

//  - --- - --- - --- - --- -

extern "C" namespace zjc {


    constexpr uint16_t maxstride    =             0xFFFF;


    cchar crk_sign[16]              =           { 0x46, 0x43, 0x4b, 0x42, 0x21, 0x54, 0x43, 0x48,
                                                  0x45, 0x53, 0x47, 0x45, 0x54, 0x24, 0x24, 0x24                        };

//  - --- - --- - --- - --- -

    class CrkFile                               {

    public:

        CrkFile()                               {                                                                       }
        ~CrkFile()                              { WARD_EVIL_MFREE(bounds); WARD_EVIL_MFREE(verts);
                                                  WARD_EVIL_MFREE(indices);                                             }

        uint16_t vertCount;
        uint16_t indexCount;

        PhysVertexPacked3D* bounds;
        VertexPacked3D* verts;
        uint16_t* indices;

                                                                                                                        };

//  - --- - --- - --- - --- -

    int openbin(cchar* filename,
                cchar* mode,
                cchar* loc,
                bool shutit)                    {

        int isnew = 0;
        curfile = fopen(filename, mode);

        if( (mode == "rb+")
         && (curfile == NULL) )                 { curfile = fopen(filename, "wb"); isnew = -1;                          }
                                                  WARD_EVIL_FOPEN(curfile, loc, filename);

        if (!shutit)                            { std::cout << "Opened file <" << filename << "> \n";                   }

        return isnew;                                                                                                   }

//  - --- - --- - --- - --- -

    int closebin(cchar* filename,
                  cchar* loc,
                  bool shutit)                  {

        int success = fclose(curfile);
        WARD_EVIL_FCLOSE(success, loc, filename);

        if (!shutit)                            { std::cout << "File closed <" << filename << "> \n";                   }

        return 0;                                                                                                       }

//  - --- - --- - --- - --- -

    int IN_writecrk(cchar* filename,
                    cchar* archive,
                    uint8_t offset,
                    uint8_t mode,
                    CrkFile* crk,
                    ushort* sizes,
                    float* bounds,
                    float* verts,
                    cchar* loc)                 {

        int errorstate = 0;

        errorstate = openbin(filename, "rb", loc);
        WARD_EVIL_WRAP(errorstate);

        fread(sizes, sizeof(uint16_t), 2, zjc::curfile);

        crk->vertCount = sizes[0];
        crk->indexCount = sizes[1];

        EVIL_FREAD(float, 24, loc, bounds);
        EVIL_FREAD(float, crk->vertCount * 8, loc, verts);
        EVIL_FREAD(std::uint16_t, crk->indexCount * 3, loc, crk->indices);

        errorstate = closebin(filename, loc);
        WARD_EVIL_WRAP(errorstate);

//  - --- - --- - --- - --- -

        crk->bounds = (PhysVertexPacked3D*) evil_malloc(8, sizeof(PhysVertexPacked3D), loc);
        crk->verts = (VertexPacked3D*) evil_malloc(*sizes, sizeof(VertexPacked3D), loc);

        for(std::uint16_t i = 0, j = 0;
            i < crk->vertCount * 8;
            i+= 8, j++ )                        { *(crk->verts+(j)) = (verts+i);                                        }

        for(std::uint16_t i = 0, j = 0;
            i < 24; i+= 3, j++ )                { *(crk->bounds+(j)) = (bounds+i);                                      }

        uint filesize = 4 + (8*6) + (crk->vertCount * 16) + (crk->indexCount * 2);

//  - --- - --- - --- - --- -

        {   int isnew = openbin(archive, "rb+", loc);
            errorstate = (isnew > -1) ? isnew : errorstate;
            WARD_EVIL_WRAP(errorstate);

            uint8_t totalfiles = 1;
            uint stride = 0;
            char sign[16];

            if(isnew)                           { EVIL_FWRITE(cchar, 16, loc, &crk_sign, archive);
                                                  EVIL_FWRITE(uint8_t, 1, loc, &totalfiles, archive);                  }


            else 
            { 
                fread(sign,
                      sizeof(char),
                      16,
                      curfile);

                for(uint i = 0; i < 16; i++)    { if(sign[i] != crk_sign[i])
                                                { terminator(0x67, loc, archive); return ERROR; }                      }
                                                  

                fread(&totalfiles,
                      sizeof(uint8_t),
                      1, curfile);

//  - --- - --- - --- - --- -

                if (offset < totalfiles) { ; }
                else if( offset == totalfiles ) { fseek(curfile, -2, SEEK_CUR);
                                                  uint newtotalfiles = totalfiles + 1;
                                                  fseek(curfile, 0, SEEK_CUR);
                                                  EVIL_FWRITE(uint8_t, 1, loc,
                                                              &newtotalfiles, archive);

                                                  fseek(curfile, 0, SEEK_CUR);
                                                  totalfiles = newtotalfiles;                                           }

                for(uint i = 0;
                    i < offset; i++)            { fread(&stride, sizeof(uint), 1, curfile);
                                                  fseek(curfile, stride, SEEK_CUR);

                                                  if(stride > maxstride)
                                                { std::cout << "Bad stride: " << stride << std::endl;
                                                  terminator(0x68, loc, archive); return 0x02; }

                                                  std::cout << "STRIDE! " << stride
                                                            << ", " << i << std::endl;                                  }
            }

        }

//  - --- - --- - --- - --- -

        EVIL_FWRITE(uint, 1, loc, &filesize, archive);
        EVIL_FWRITE(uint16_t, 2, loc, sizes, archive);
        EVIL_FWRITE(PhysVertexPacked3D, 8, loc, crk->bounds, archive);
        EVIL_FWRITE(VertexPacked3D, crk->vertCount, loc, crk->verts, archive);
        EVIL_FWRITE(uint16_t, crk->indexCount, loc, crk->indices, archive);
        
        errorstate = closebin(archive, loc);
        WARD_EVIL_WRAP(errorstate);

        std::remove(filename);
        std::cout << "Deleted file " << filename << std::endl;

        return 0;
                                                                                                                        }

//  - --- - --- - --- - --- -

    int writecrk(cchar* filename,
                 cchar* archive,
                 char* offset,
                 char* mode,
                 cchar* loc)                 {

        CrkFile* crk = new CrkFile;

        ushort sizes[2] = {0, 0};
        float* bounds = nullptr;
        float* verts = nullptr;

        uint8_t numoffset = HEXLIT(offset, uint8_t);
        uint8_t nummode   = HEXLIT(mode,   uint8_t);

        int evilstate = IN_writecrk(filename, archive, numoffset, nummode,
                                    crk, sizes, bounds, verts, loc);

        delete crk;

        WARD_EVIL_MFREE(bounds);
        WARD_EVIL_MFREE(verts);

        return evilstate;                                                                                               }

//  - --- - --- - --- - --- -

}