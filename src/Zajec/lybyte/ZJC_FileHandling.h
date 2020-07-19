#ifndef __ZJC_FileHandling_H__
#define __ZJC_FileHandling_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ZJC_BinTypes.h"

//  - --- - --- - --- - --- -

typedef struct DSM_ARCHIVE_FORMAT DAF;

typedef struct MESH_FILE_3D CrkFile;
typedef struct IMAGE_FILE_8BIT_RGB JojFile;

int    writecrk         (cchar* filename, cchar* archive, char* mode, char* offset);
int    writejoj         (cchar* filename, cchar* archive, char* mode, char* offset);

int    extraction_start (cchar* filename, uchar archtype, DAF* daf);
int    extraction_end   (cchar* filename);

int    extractcrk       (DAF* daf, uchar offset, ushort* vertCount,
                         ushort* indexCount, pVP3D_8* bounds,
                         VP3D_8* verts, ushort* indices);

int    extractjoj       (DAF* daf, uchar offset, ushort* size, uchar* width, uchar* height, ushort* pixels);

#ifdef __cplusplus
}
#endif

#endif //__ZJC_FileHandling_H__
