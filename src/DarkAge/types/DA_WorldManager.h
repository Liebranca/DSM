#ifndef __DARKAGE_WORLDMANAGER_H__
#define __DARKAGE_WORLDMANAGER_H__

#include "ZJC_CommonTypes.h"
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

#define __DA_MAX_OBJECTS_PER_CELL__ 128
#define   DA_CELL_SIZE              8

struct DA_CELL {

    int    worldpos[2];
    int    inFrustum;

    uint   edge;
    uchar  num_objects;
    ushort shuffle_loc;
    ushort oblocs[__DA_MAX_OBJECTS_PER_CELL__];
};

typedef struct DA_CELL_LOOKUP {

    uint     found;
    DA_CELL* cells[9];

} DACL;

//  - --- - --- - --- - --- -

void DA_grid_init          (int width, int height);
void DA_grid_end           ();
void DA_grid_modWorldOffset(int mvec[2]);
void DA_grid_calcRugPull   (int mvec[2]);

void DA_grid_regObject     (int pos[2], int ipos[3], ushort obloc, int is_dynamic);
void DA_grid_unregObject   (int ipos[3], int is_dynamic);
void DA_grid_fetchOblocs   (int origin[2], int* cell_positions, uint* cellCounter, uint* obCounter, ushort* locations);
void DA_grid_cullCell      (int pos[2], int is_culled);
int  DA_grid_getInFrustum  (int pos[2]);

void DA_grid_setSimRadius  (uint radius);
uint DA_grid_getFrustumFac ();

void DA_grid_findpos       (int dest[2], float pos[3]);
void DA_grid_getsurround   (DACL* cellLookup, int pos[2]);

void DA_debug_drawgrid     (int pos[2]);
void DA_debug_drawgrid_sur (DACL* cellLookup);
void DA_debug_drawgrid_cull(DACL* cellLookup);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __DARKAGE_WORLDMANAGER_H__
