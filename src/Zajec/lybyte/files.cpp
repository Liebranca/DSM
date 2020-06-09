#include <iostream>

#include "files.h"
#include "types.h"

#include "../lymath/gops.h"
#include "../lyutils/evil.h"

//  - --- - --- - --- - --- -

#define EVIL_FREAD(type, count, loc, buff)        buff = (type*) zjc::evil_malloc(count, sizeof(type), loc);\
                                                  fread(buff, sizeof(type), count, zjc::curfile);

#define EVIL_FWRITE(type, count, loc, buff, info) WARD_EVIL_FWRITE(fwrite(buff, sizeof(type),\
                                                  count, curfile), loc, info, count);

//  - --- - --- - --- - --- -

namespace zjc {

    void openbin(concha* filename,
                 concha* mode,
                 concha* loc)                   { curfile = fopen(filename, mode);
                                                  WARD_EVIL_FOPEN(curfile, loc, filename);
                                                  std::cout << "Opened file <" << filename << "> \n";                   }

    void closebin(concha* filename,
                  concha* loc)                  { int success = fclose(curfile);
                                                  WARD_EVIL_FCLOSE(success, loc, filename);
                                                  std::cout << "File closed <" << filename << "> \n";                   }

//  - --- - --- - --- - --- -

    void writecrk(concha* filename,
                  concha* loc)                  {
        
        std::uint16_t* sizes;
        std::uint16_t* indices;
        float* bounds;
        float* verts;
        
        openbin(filename, "r", loc);

        EVIL_FREAD(std::uint16_t, 2, loc, sizes);
        EVIL_FREAD(float, 24, loc, bounds);
        EVIL_FREAD(float, (*sizes) * 8, loc, verts);
        EVIL_FREAD(std::uint16_t, (*(sizes+1)) * 3, loc, indices);

        closebin(filename, loc);

        VertexPacked3D* packed_verts;
        packed_verts = (VertexPacked3D*) evil_malloc(*sizes, sizeof(VertexPacked3D), loc);

        for(std::uint16_t i = 0, j = 0;
            i < (*sizes) * 8; i+= 8, j++ )      { *(packed_verts+(j)) = (verts+i);                                      }

        PhysVertexPacked3D* packed_bounds;
        packed_bounds = (PhysVertexPacked3D*) evil_malloc(8, sizeof(PhysVertexPacked3D), loc);

        for(std::uint16_t i = 0, j = 0;
            i < 24; i+= 3, j++ )                { *(packed_bounds+(j)) = (bounds+i);                                    }

        openbin(filename, "w", loc);
        EVIL_FWRITE(uint16_t, 2, loc, sizes, filename);
        EVIL_FWRITE(PhysVertexPacked3D, 8, loc, packed_bounds, filename);
        EVIL_FWRITE(VertexPacked3D, *sizes, loc, packed_verts, filename);
        EVIL_FWRITE(uint16_t, *(sizes+1), loc, indices, filename);
        closebin(filename, loc);

        evil_free(&sizes);
        evil_free(&bounds);
        evil_free(&verts);
        evil_free(&indices);

        evil_free(&packed_verts);
        evil_free(&packed_bounds);

                                                                                                                        }

}