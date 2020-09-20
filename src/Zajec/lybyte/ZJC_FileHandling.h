#ifndef __ZJC_FileHandling_H__
#define __ZJC_FileHandling_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ZJC_BinTypes.h"

//  - --- - --- - --- - --- -

typedef struct DSM_ARCHIVE_FORMAT DAF;

typedef struct MESH_FILE_3D CRK;
typedef struct IMAGE_FILE_24BIT_YUV JOJ;

typedef struct SCENE_OBJECT                     {

    ushort resinfo[2];
    float  transform[10];

} SSO;

typedef struct SCENE_OBJECTS_FILE               {

    ushort count;
    SSO*   objects;

} SSX;

int    writecrk         (cchar* filename, cchar* archive, char* mode, char* offset);
int    writejoj         (cchar* filename, cchar* archive, char* mode, char* offset);

int    extraction_start (cchar* filename, uchar archtype, DAF** daf);
int    extraction_end   (cchar* filename, DAF** daf);

int    extractcrk       (DAF* daf, uchar offset, ushort* vertCount,
                         ushort* indexCount, pVP3D_8** bounds,
                         VP3D_8** verts, ushort** indices);

int    extractjoj       (DAF* daf, uchar offset, uint* size, ushort* width, ushort* height, float** pixels);

int    readssx          (SSX* ssx, cchar* filename);
void   del_SsxFile      (SSX* ssx);

DAF*   dafalloc();

#ifdef __cplusplus
}
#endif

#endif //__ZJC_FileHandling_H__
