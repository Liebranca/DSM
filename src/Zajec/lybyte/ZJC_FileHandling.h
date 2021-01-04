#ifndef __ZJC_FileHandling_H__
#define __ZJC_FileHandling_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ZJC_BinTypes.h"

//  - --- - --- - --- - --- -

typedef struct ZJC_BINFILE {

    cchar* name;
    FILE*  file;

} ZBIN;

typedef struct DARK_AGE_FILE                    {

    uchar    sign[8];
    uint     fileCount;
    uint     size;
    uint     offsets[ZJC_DAFSIZE];

} DAF;

//  - --- - --- - --- - --- -

typedef struct MESH_FILE_3D                     {

    ushort boxCount;
    ushort frameCount;
    ushort vertCount;
    ushort triCount;

    uint   matid;
    uint   numBinds;

    float  binds[48];

} CRK;

typedef struct MESH_DATA_3D {

    BP3D*   boxes;
    ushort* indices;

    float*  bindsmat;
    VP3D*   verts;

} MD3D;

//  - --- - --- - --- - --- -

typedef struct TEXTURE_FILE_24BITYUVA           {

    ushort  height;
    ushort  width;

    uint    imcount;
    uint    datasize_i;
    uint    datasize_d;

} JOJ;

typedef struct JOJ_MATERIAL_BLOCK {

    float   spec_mult;
    float   diff_mult;
    float   ref_mult;
    float   glow_rea;

    ushort  shdid;
    ushort  flags;

} JOJMATE;

//  - --- - --- - --- - --- -

typedef struct SCENE_OBJECT                     {

    ushort resinfo[ 3];
    float  fvalues[22];

} SSO;

typedef struct SCENE_OBJECTS_FILE               {

    ushort count;
    SSO*   objects;

} SSX;

//  - --- - --- - --- - --- -

int    openbin          (ZBIN* bin, cchar* mode, int shutit);
int    closebin         (ZBIN* bin, int shutit);

int    writecrk         (cchar* filename, cchar* archive, char* mode, char* offset);
int    writejoj         (cchar* filename, cchar* archive, char* mode, char* offset);

int    ZJC_open_daf     (ZBIN* src, uchar archtype);
int    ZJC_close_daf    (ZBIN* src, int isLast);

void   ZJC_init_crk     ();
void   ZJC_end_crk      ();

MD3D*  ZJC_get_crkdata  ();
CRK*   ZJC_get_curcrk   ();

int    ZJC_extract_crk  (ZBIN* src, ZBIN* storage, uchar offset, uchar start);
int    ZJC_read_crkframe(ZBIN* storage);

/* outdated func. rewrite
int    extractjoj       (DAF* daf, uchar offset, uchar* imcount, uint* size, ushort* width,
                         ushort* height, uchar* flags, float fvalues[4], ushort* shdid);*/

void   joj_subTexRead   (uint dim, uchar imcount, uchar curim, float** pixels);

int    readssx          (SSX* ssx, cchar* filename);
void   del_SsxFile      (SSX* ssx);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif //__ZJC_FileHandling_H__
