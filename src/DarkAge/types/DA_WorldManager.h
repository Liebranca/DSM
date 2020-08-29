#ifndef __DARKAGE_WORLDMANAGER_H__
#define __DARKAGE_WORLDMANAGER_H__

#include "../DA_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

#define    __DA_MAX_OBJECTS_PER_CELL__ 128
#define      DA_CELL_SIZE              8

static cuint DA_CELL_HALF            = DA_CELL_SIZE / 2;

typedef struct DA_GRIDCELL {

    int    worldpos[2];
    int    inFrustum;

    uint   edge;
    uchar  num_objects;
    ushort shuffle_loc;

    ushort oblocs[__DA_MAX_OBJECTS_PER_CELL__];

} DA_CELL;

typedef struct DA_CELL_LOOKUP {

    uint     found;
    DA_CELL* cells[9];

} DACL;

//  - --- - --- - --- - --- -

void DA_grid_init          (int width, int height);
void DA_grid_end           ();
void DA_grid_modWorldOffset(int mvec[2]);
void DA_grid_calcRugPull   (int mvec[2]);

void DA_grid_regObject     (DANCI* nci, ushort obloc);
void DA_grid_unregObject   (DANCI* nci, int    ret[2]);
void DA_grid_fetchOblocs   (int origin[2], DAGCI* camcells, uint* cellCounter, uint* obCounter, ushort* locations);
void DA_grid_setInFrustum  (uint gridpos[2], int cellInFrustum);
int  DA_grid_getInFrustum  (uint gridpos[2]);

void DA_grid_setSimRadius  (uint radius);
uint DA_grid_getFrustumFac ();

void DA_grid_findpos       (int dest[2], float pos[3]);
void DA_grid_findabspos    (uint dest[2], int pos[2]);
void DA_grid_getsurround   (DACL* cellLookup, uint gridpos[2]);

void DA_debug_drawgrid     (int pos[2]);
void DA_debug_drawgrid_sur (DACL* cellLookup);
void DA_debug_drawgrid_cull(DACL* cellLookup);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __DARKAGE_WORLDMANAGER_H__
