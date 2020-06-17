#ifndef __ZJC_FileHandling_H__
#define __ZJC_FileHandling_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ZJC_BinTypes.h"

//  - --- - --- - --- - --- -

typedef struct DSM_ARCHIVE_FORMAT DAF;

typedef struct MESH_FILE_3D CrkFile;
typedef struct MESH_FILE_2D HrnFile;

int    writecrk         (cchar* filename, cchar* archive, char* offset, char* mode);

int    extraction_start (cchar* filename, uchar archtype, DAF* daf);
int    extraction_end   (cchar* filename);

int    extractcrk       (DAF* daf, uchar offset, ushort* vertCount,
                         ushort* indexCount, pVP3D* bounds,
                         VP3D* verts, ushort* indices);

#ifdef __cplusplus
}
#endif

#endif //__ZJC_FileHandling_H__
