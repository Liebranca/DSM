#ifndef __DARKAGE_WORLDMANAGER_H__
#define __DARKAGE_WORLDMANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../DA_CommonTypes.h"

//  - --- - --- - --- - --- -

#define    __DA_MAX_OBJECTS_PER_CELL__ 128
#define      DA_CELL_SIZE              8

static cuint DA_CELL_HALF            = DA_CELL_SIZE / 2;
static cuint DA_CELL_QUAT            = DA_CELL_SIZE / 4;

typedef struct DA_GRIDCELL {

    int    worldpos[3];
    int    inFrustum;

    uchar  num_objects;
    ushort shuffle_loc;

    ushort oblocs[__DA_MAX_OBJECTS_PER_CELL__];

} DA_CELL;

typedef struct DA_CELL_LOOKUP {

    uint     found;
    DA_CELL* cells[27];

} DACL;

//  - --- - --- - --- - --- -

void DA_grid_init          (int xwidth, int zwidth, int height);
void DA_grid_end           ();
void DA_grid_modWorldOffset(int mvec[3]);
void DA_grid_calcRugPull   (int mvec[3]);

void DA_grid_regObject     (DANCI* nci, ushort obloc);
void DA_grid_unregObject   (DANCI* nci, int    ret[2]);
void DA_grid_fetchOblocs   (int origin[3], DAGCI* camcells, uint* cellCounter, uint* obCounter, ushort* locations);
void DA_grid_setInFrustum  (uint gridpos[3], int cellInFrustum);
int  DA_grid_getInFrustum  (uint gridpos[3]);

void DA_grid_setSimRadius  (uint radius);
uint DA_grid_getFrustumFac ();

void DA_grid_findpos       (int dest[3], float pos[3]);
void DA_grid_findabspos    (uint dest[3], int pos[3]);
void DA_grid_getsurround   (DACL* cellLookup, uint gridpos[3]);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __DARKAGE_WORLDMANAGER_H__
