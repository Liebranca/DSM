#include "DA_WorldManager.h"

#include "lyutils/ZJC_Evil.h"
#include "lymath/ZJC_GOPS.h"

#include <stdio.h>
#include <math.h>

static int        DA_GRID_XWIDTH     = 1;
static int        DA_GRID_ZWIDTH     = 1;
static int        DA_GRID_HEIGHT     = 1;

static uint       DA_GRID_XFAC       = 0;
static uint       DA_GRID_ZFAC       = 0;
static uint       DA_GRID_YFAC       = 0;

static uint       DA_SIM_RADIUS      = 2 * DA_CELL_SIZE;
static int        DA_WORLD_OFFSET[3] = { 0, 0, 0 };

const float       DA_CELL_DIV        = 1.0f/DA_CELL_SIZE;

static DA_CELL*** DA_GRID            = NULL;
static ushort*    DA_ACTIVE_CELL_IDS = NULL;

static ushort     DA_ACTIVE_CELLS    = 0;

//  - --- - --- - --- - --- -

void DA_grid_makecell(DA_CELL** cell,
                      int x, int y, int z)      { (*cell)->num_objects = 0; (*cell)->worldpos[0] = x;
                                                  (*cell)->worldpos[1] = y; (*cell)->worldpos[2] = z;                   }

void DA_grid_end ()                             {

    WARD_EVIL_MFREE(DA_ACTIVE_CELL_IDS);

    for(uint y = 0; y < DA_GRID_YFAC; y++)
    {
        for(uint z = 0; z < DA_GRID_ZFAC; z++)
        {
            WARD_EVIL_MFREE(DA_GRID[y][z]);
        }
        WARD_EVIL_MFREE(DA_GRID[y]);
    }

    WARD_EVIL_MFREE(DA_GRID);                                                                                           }

void DA_grid_init(int xwidth,
                  int zwidth,
                  int height )                  {

    if(DA_GRID != NULL) { DA_grid_end(); }

    DA_GRID_XWIDTH = xwidth;
    DA_GRID_ZWIDTH = zwidth;
    DA_GRID_HEIGHT = height;

    DA_GRID_XFAC   = 0; for(int x = -xwidth ; x < (xwidth  + 1); x++) { DA_GRID_XFAC++; }
    DA_GRID_ZFAC   = 0; for(int z = -zwidth ; z < (zwidth  + 1); z++) { DA_GRID_ZFAC++; }
    DA_GRID_YFAC   = 0; for(int y = -height;  y < (height  + 1); y++) { DA_GRID_YFAC++; }

    DA_GRID = (DA_CELL***) evil_malloc(DA_GRID_YFAC, sizeof(DA_CELL**));
    for(uint y = 0; y < DA_GRID_YFAC; y++)
    {
        DA_GRID[y] = (DA_CELL**)evil_malloc(DA_GRID_ZFAC, sizeof(DA_CELL*));
        for(uint z = 0; z < DA_GRID_ZFAC; z++)
        {
            DA_GRID[y][z] = (DA_CELL*)evil_malloc(DA_GRID_XFAC, sizeof(DA_CELL));
        }
    }

    for(int y = -height; y < (height + 1); y++)
    {
        for(int z = -zwidth; z < (zwidth + 1); z++)
        {
            for(int x = -xwidth; x < (xwidth + 1); x++)
            {
                int wy  = y + height; int wz  = z + zwidth; int wx  = x + xwidth;
                DA_CELL* cell = &DA_GRID[wy][wz][wx]; DA_grid_makecell(&cell, x, y, z);
            }
        }
    }

    DA_ACTIVE_CELL_IDS = (ushort*) evil_malloc(DA_GRID_YFAC * DA_GRID_ZFAC * DA_GRID_XFAC, sizeof(ushort));             }

//  - --- - --- - --- - --- -

void DA_grid_modWorldOffset(int mvec[3])        { DA_WORLD_OFFSET[0] += clampi(mvec[0], -1, 1);
                                                  DA_WORLD_OFFSET[1] += clampi(mvec[1], -1, 1);
                                                  DA_WORLD_OFFSET[2] += clampi(mvec[2], -1, 1);                         }

void DA_grid_calcRugPull   (int mvec[3])        { if(mvec[0]) { mvec[0] = mvec[0] * DA_CELL_SIZE; }
                                                  if(mvec[1]) { mvec[1] = mvec[1] * DA_CELL_SIZE; }
                                                  if(mvec[2]) { mvec[2] = mvec[2] * DA_CELL_SIZE; }                     }

//  - --- - --- - --- - --- -

void DA_grid_setSimRadius  (uint radius)        { DA_SIM_RADIUS = radius * DA_CELL_SIZE;                                }

uint DA_grid_getFrustumFac ()                   {

    uint maxfac = maxi(maxi(DA_GRID_XFAC, DA_GRID_ZFAC), DA_GRID_YFAC);
    return (uint)pow(maxfac, 2);                                                                                        }

void DA_grid_setInFrustum (uint gridpos[3],
                           int cellInFrustum)   { DA_CELL* cell   = &DA_GRID[gridpos[1]][gridpos[2]][gridpos[0]];
                                                  cell->inFrustum = cellInFrustum;                                      }

int  DA_grid_getInFrustum (uint gridpos[3])     { DA_CELL* cell = &DA_GRID[gridpos[1]][gridpos[2]][gridpos[0]];
                                                  return   cell->inFrustum;                                             }

void DA_grid_setInLightFrustum
(uint gridpos[3],
 uint lampid,
 int  cellInLightFrustum)                       { DA_CELL* cell     = &DA_GRID[gridpos[1]][gridpos[2]][gridpos[0]];
                                                  cell->LIT[lampid] = cellInLightFrustum;                               }

int  DA_grid_getInLightFrustum
(uint gridpos[3], uint lampid)                  { DA_CELL* cell = &DA_GRID[gridpos[1]][gridpos[2]][gridpos[0]];
                                                  return   cell->LIT[lampid];                                           }

//  - --- - --- - --- - --- -

void DA_grid_regObject     (DANCI* nci,
                            ushort obloc)       {

    nci->gridpos[0]  = nci->worldpos[0] + DA_GRID_XWIDTH;
    nci->gridpos[1]  = nci->worldpos[1] + DA_GRID_HEIGHT;
    nci->gridpos[2]  = nci->worldpos[2] + DA_GRID_ZWIDTH ;

    DA_CELL* cell = &DA_GRID[nci->gridpos[1]][nci->gridpos[2]][nci->gridpos[0]];

    if(cell->num_objects == 0 )                 { DA_ACTIVE_CELL_IDS[DA_ACTIVE_CELLS] =  nci->gridpos[0]
                                                                                      + (nci->gridpos[1] <<  5)
                                                                                      + (nci->gridpos[2] << 10);

                                                  cell->shuffle_loc                   = DA_ACTIVE_CELLS;
                                                  DA_ACTIVE_CELLS++;                                                    }

    nci->index               = cell->num_objects;
    cell->oblocs[nci->index] = obloc;

    cell->num_objects++;                                                                                                }

void DA_grid_unregObject   (DANCI* nci,
                            int    ret[2])      {

    DA_CELL* cell = &DA_GRID[nci->gridpos[1]][nci->gridpos[0]][nci->gridpos[2]];
    cell->num_objects--;

    uint last_index = cell->num_objects;

    if(last_index != nci->index)                { cell->oblocs[nci->index] = cell->oblocs[last_index];
                                                  ret[0]                   = cell->oblocs[nci->index];                  }

    else                                        { ret[1]                   = 0;                                         }

    if(cell->num_objects == 0)                  { DA_ACTIVE_CELLS--;

                                                  if(cell->shuffle_loc != DA_ACTIVE_CELLS)
                                                { DA_ACTIVE_CELL_IDS[cell->shuffle_loc] 
                                                  = DA_ACTIVE_CELL_IDS[DA_ACTIVE_CELLS];                                }

                                                  DA_ACTIVE_CELL_IDS[DA_ACTIVE_CELLS] = 0;                              }

    cell->oblocs[last_index] = 0;                                                                                       }

void DA_grid_fetchOblocs   (int     origin[3],
                            DAGCI*  camcells,
                            uint*   cellCounter,
                            uint*   obCounter,
                            ushort* locations)  {

    for(uint i = 0; i < DA_ACTIVE_CELLS; i++)
    {

        uint     cx      =  DA_ACTIVE_CELL_IDS[i] & 0b0000000000011111;
        uint     cy      = (DA_ACTIVE_CELL_IDS[i] & 0b0000001111100000) >>  5;
        uint     cz      = (DA_ACTIVE_CELL_IDS[i] & 0b0111110000000000) >> 10;

        DA_CELL* cell    = &DA_GRID[cy][cz][cx];

        if(  iv2_distance(cell->worldpos,
                          origin          )

          <= DA_SIM_RADIUS                )     { DAGCI* gci        = camcells + (*cellCounter);

                                                  gci->worldpos[0]  = clampi(cell->worldpos[0] - DA_WORLD_OFFSET[0],
                                                                             -DA_GRID_XWIDTH, DA_GRID_XWIDTH         );

                                                  gci->worldpos[1]  = clampi(cell->worldpos[1] - DA_WORLD_OFFSET[1],
                                                                             -DA_GRID_HEIGHT, DA_GRID_HEIGHT         );

                                                  gci->worldpos[2]  = clampi(cell->worldpos[2] - DA_WORLD_OFFSET[2],
                                                                             -DA_GRID_ZWIDTH, DA_GRID_ZWIDTH         );

                                                  gci->gridpos [0]  = cx;
                                                  gci->gridpos [1]  = cy;
                                                  gci->gridpos [2]  = cz;
                                                  *cellCounter     += 1;

                                                  for(uint j = 0; j < cell->num_objects; j++)
                                                { locations[*obCounter] = cell->oblocs[j]; (*obCounter) += 1; }         }
    }                                                                                                                   }

//  - --- - --- - --- - --- -

void DA_grid_findpos      (int   dest[3],
                           float pos[3])        {

    dest[0] = (int)truncf(pos[0] * DA_CELL_DIV);
    dest[1] = (int)truncf(pos[1] * DA_CELL_DIV);
    dest[2] = (int)truncf(pos[2] * DA_CELL_DIV);

    int xdom[2] = { (dest[0] * DA_CELL_SIZE) - DA_CELL_HALF, (dest[0] * DA_CELL_SIZE) + DA_CELL_HALF };
    int ydom[2] = { (dest[1] * DA_CELL_SIZE) - DA_CELL_HALF, (dest[1] * DA_CELL_SIZE) + DA_CELL_HALF };
    int zdom[2] = { (dest[2] * DA_CELL_SIZE) - DA_CELL_HALF, (dest[2] * DA_CELL_SIZE) + DA_CELL_HALF };

    if     (pos[0] < xdom[0]) { dest[0] -= 1; }
    else if(pos[0] > xdom[1]) { dest[0] += 1; }

    if     (pos[1] < ydom[0]) { dest[1] -= 1; }
    else if(pos[1] > ydom[1]) { dest[1] += 1; }

    if     (pos[2] < zdom[0]) { dest[2] -= 1; }
    else if(pos[2] > zdom[1]) { dest[2] += 1; }

    dest[0] = clampi(dest[0] + DA_WORLD_OFFSET[0], -DA_GRID_XWIDTH, DA_GRID_XWIDTH);
    dest[2] = clampi(dest[2] + DA_WORLD_OFFSET[2], -DA_GRID_ZWIDTH, DA_GRID_ZWIDTH);
    dest[1] = clampi(dest[1] + DA_WORLD_OFFSET[1], -DA_GRID_HEIGHT, DA_GRID_HEIGHT);                                    }

void DA_grid_findabspos   (uint dest[3],
                           int  pos [3] )       { dest[0]  = pos[0] + DA_GRID_XWIDTH;
                                                  dest[2]  = pos[2] + DA_GRID_ZWIDTH;
                                                  dest[1]  = pos[1] + DA_GRID_HEIGHT;                                   }

void DA_grid_getsurround  (DACL* cellLookup,
                           uint gridpos[3])     {

    DA_CELL* center      = &DA_GRID[gridpos[1]][gridpos[2]][gridpos[0]];

    cellLookup->cells[0] = center;
    cellLookup->found    = 1;

    for(int y = -1; y < 2; y+=2)
    {
        for(int z = -1; z < 2; z+=2)
        {
            for(int x = -1; x < 2; x+=2)
            {
                int cx = gridpos[0] + x;
                int cy = gridpos[1] + y;
                int cz = gridpos[2] + z;

                if( (cx < (int)DA_GRID_XFAC && cx >= 0)
                &&  (cy < (int)DA_GRID_YFAC && cy >= 0)
                &&  (cz < (int)DA_GRID_ZFAC && cz >= 0) ) { cellLookup->cells[cellLookup->found] = &DA_GRID[cy][cz][cx];
                                                            cellLookup->found++;                                        }
            }
        }
    }
                                                                                                                        }

//  - --- - --- - --- - --- -
