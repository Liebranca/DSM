#include "DA_WorldManager.h"

#include "lyutils/ZJC_Evil.h"
#include "lymath/ZJC_GOPS.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static int       DA_GRID_HEIGHT     = 1;
static int       DA_GRID_WIDTH      = 1;
static uint      DA_GRID_YFAC       = 0;
static uint      DA_GRID_XFAC       = 0;

static uint      DA_SIM_RADIUS      = 2 * DA_CELL_SIZE;
static int       DA_WORLD_OFFSET[2] = { 0, 0 };

const float      DA_CELL_DIV        = 1.0f/DA_CELL_SIZE;

static DA_CELL** DA_GRID            = NULL;
static ushort*   DA_ACTIVE_CELL_IDS = NULL;

static ushort    DA_ACTIVE_CELLS    = 0;

//  - --- - --- - --- - --- -

void DA_grid_makecell(DA_CELL** cell,
                      int x, int y)             {

    uint go_right        = 1 * (x > -(DA_GRID_WIDTH));
    uint go_left         = 2 * (x <  (DA_GRID_WIDTH));
    uint go_down         = 4 * (y > -(DA_GRID_HEIGHT));
    uint go_up           = 8 * (y <  (DA_GRID_HEIGHT));

    (*cell)->num_objects = 0;
    (*cell)->worldpos[0] = x;
    (*cell)->worldpos[1] = y;

    (*cell)->edge        = (go_right | go_left | go_down | go_up);                                                      }

void DA_grid_end ()                             { WARD_EVIL_MFREE(DA_ACTIVE_CELL_IDS);

                                                  for(uint i = 0; i < DA_GRID_YFAC; i++)
                                                { WARD_EVIL_MFREE(DA_GRID[i]);  } WARD_EVIL_MFREE(DA_GRID);             }

void DA_grid_init(int width, int height)        {

    if(DA_GRID != NULL) { DA_grid_end(); }

    DA_GRID_WIDTH  = width;
    DA_GRID_HEIGHT = height;

    DA_GRID_XFAC   = 0; for(int x = -width ; x < (width  + 1); x++) { DA_GRID_XFAC++; }
    DA_GRID_YFAC   = 0; for(int y = -height; y < (height + 1); y++) { DA_GRID_YFAC++; }

    DA_GRID = (DA_CELL**) evil_malloc(DA_GRID_YFAC, sizeof(DA_CELL*));
    for(uint i = 0; i < DA_GRID_YFAC; i++)      { DA_GRID[i] = (DA_CELL*)evil_malloc(DA_GRID_XFAC, sizeof(DA_CELL));    }

    for(int y = -height; y < (height + 1); y++)
    {

        for(int x = -width;
            x < (width + 1); x++)               { int wx  = x + width; int wy  = y + height;
                                                  DA_CELL* cell = &DA_GRID[wy][wx]; DA_grid_makecell(&cell, x, y);      }

    }

    DA_ACTIVE_CELL_IDS = (ushort*) evil_malloc(height * width * 2, sizeof(ushort));                                     }

//  - --- - --- - --- - --- -

void DA_grid_modWorldOffset(int mvec[2])        { DA_WORLD_OFFSET[0] += clampi(mvec[0], -1, 1);
                                                  DA_WORLD_OFFSET[1] += clampi(mvec[1], -1, 1);                         }

void DA_grid_calcRugPull   (int mvec[2])        { if(mvec[0]) { mvec[0] = mvec[0] * DA_CELL_SIZE; }
                                                  if(mvec[1]) { mvec[1] = mvec[1] * DA_CELL_SIZE; }                     }

//  - --- - --- - --- - --- -

void DA_grid_setSimRadius  (uint radius)        { DA_SIM_RADIUS = radius * DA_CELL_SIZE;                                }
uint DA_grid_getFrustumFac ()                   { return (uint)pow(maxi(DA_GRID_YFAC, DA_GRID_XFAC), 2);                }

void DA_grid_setInFrustum (uint gridpos[2],
                           int cellInFrustum)   { DA_CELL* cell   = &DA_GRID[gridpos[1]][gridpos[0]];
                                                  cell->inFrustum = cellInFrustum;                                      }

int  DA_grid_getInFrustum (uint gridpos[2])     { DA_CELL* cell = &DA_GRID[gridpos[1]][gridpos[0]];
                                                  return   cell->inFrustum;                                             }

//  - --- - --- - --- - --- -

void DA_grid_regObject     (DANCI* nci,
                            ushort obloc)       {

    nci->gridpos[0]  = nci->worldpos[0] + DA_GRID_WIDTH ;
    nci->gridpos[1]  = nci->worldpos[1] + DA_GRID_HEIGHT;

    DA_CELL* cell = &DA_GRID[nci->gridpos[1]][nci->gridpos[0]];

    if(cell->num_objects == 0 )                 { DA_ACTIVE_CELL_IDS[DA_ACTIVE_CELLS] = nci->gridpos[0]
                                                                                      + (nci->gridpos[1] << 8);

                                                  cell->shuffle_loc                   = DA_ACTIVE_CELLS;
                                                  DA_ACTIVE_CELLS++;                                                    }

    nci->index               = cell->num_objects;
    cell->oblocs[nci->index] = obloc;

    cell->num_objects++;                                                                                                }

void DA_grid_unregObject   (DANCI* nci,
                            int    ret[2])      {

    DA_CELL* cell = &DA_GRID[nci->gridpos[1]][nci->gridpos[0]];
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

void DA_grid_fetchOblocs   (int     origin[2],
                            DAGCI*  camcells,
                            uint*   cellCounter,
                            uint*   obCounter,
                            ushort* locations)  {

    for(uint i = 0; i < DA_ACTIVE_CELLS; i++)
    {

        uint     cx      =  DA_ACTIVE_CELL_IDS[i] & 0b0000000011111111;
        uint     cy      = (DA_ACTIVE_CELL_IDS[i] & 0b1111111100000000) >> 8;
        DA_CELL* cell    = &DA_GRID[cy][cx];

        if(  iv2_distance(cell->worldpos,
                          origin          )

          <= DA_SIM_RADIUS                )     { DAGCI* gci        = camcells + (*cellCounter);

                                                  gci->worldpos[0]  = clampi(cell->worldpos[0] - DA_WORLD_OFFSET[0],
                                                                             -DA_GRID_WIDTH, DA_GRID_WIDTH           );

                                                  gci->worldpos[1]  = clampi(cell->worldpos[1] - DA_WORLD_OFFSET[1],
                                                                             -DA_GRID_HEIGHT, DA_GRID_HEIGHT         );

                                                  gci->gridpos [0]  = cx;
                                                  gci->gridpos [1]  = cy;
                                                  *cellCounter     += 1;

                                                  for(uint j = 0; j < cell->num_objects; j++)
                                                { locations[*obCounter] = cell->oblocs[j]; (*obCounter) += 1; }         }
    }                                                                                                                   }

//  - --- - --- - --- - --- -

void DA_grid_findpos      (int   dest[2],
                           float pos[3])        {

    dest[0] = (int)truncf(pos[0] * DA_CELL_DIV);
    dest[1] = (int)truncf(pos[2] * DA_CELL_DIV);

    int xdom[2] = { (dest[0] * DA_CELL_SIZE) - DA_CELL_HALF, (dest[0] * DA_CELL_SIZE) + DA_CELL_HALF };
    int ydom[2] = { (dest[1] * DA_CELL_SIZE) - DA_CELL_HALF, (dest[1] * DA_CELL_SIZE) + DA_CELL_HALF };

    if     (pos[0] < xdom[0]) { dest[0] -= 1; }
    else if(pos[0] > xdom[1]) { dest[0] += 1; }

    if     (pos[2] < ydom[0]) { dest[1] -= 1; }
    else if(pos[2] > ydom[1]) { dest[1] += 1; }

    dest[0] = clampi(dest[0] + DA_WORLD_OFFSET[0], -DA_GRID_WIDTH,  DA_GRID_WIDTH );
    dest[1] = clampi(dest[1] + DA_WORLD_OFFSET[1], -DA_GRID_HEIGHT, DA_GRID_HEIGHT);                                    }

void DA_grid_findabspos   (uint dest[2],
                           int  pos[2]  )       { dest[0]  = pos[0] + DA_GRID_WIDTH ;
                                                  dest[1]  = pos[1] + DA_GRID_HEIGHT;                                   }

void DA_grid_getsurround  (DACL* cellLookup,
                           uint gridpos[2])     {

    DA_CELL* center      = &DA_GRID[gridpos[1]][gridpos[0]];

    cellLookup->cells[0] = center;
    cellLookup->found    = 1;

    if( center->edge & 1 )                      { cellLookup->cells[cellLookup->found]
                                                  = &DA_GRID[gridpos[1]][gridpos[0] - 1];

                                                  cellLookup->found++;                                                  }

    if( center->edge & 2 )                      { cellLookup->cells[cellLookup->found]
                                                  = &DA_GRID[gridpos[1]][gridpos[0] + 1];

                                                  cellLookup->found++;                                                  }

    if( center->edge & 4 )                      { cellLookup->cells[cellLookup->found]
                                                  = &DA_GRID[gridpos[1] - 1][gridpos[0]];

                                                  cellLookup->found++;                                                  }

    if( center->edge & 8 )                      { cellLookup->cells[cellLookup->found]
                                                  = &DA_GRID[gridpos[1] + 1][gridpos[0]];

                                                  cellLookup->found++;                                                  }

    if((center->edge & 1) && (center->edge & 4)){ cellLookup->cells[cellLookup->found]
                                                  = &DA_GRID[gridpos[1] - 1][gridpos[0] - 1];

                                                  cellLookup->found++;                                                  }

    if((center->edge & 2) && (center->edge & 4)){ cellLookup->cells[cellLookup->found]
                                                  = &DA_GRID[gridpos[1] - 1][gridpos[0] + 1];

                                                  cellLookup->found++;                                                  }

    if((center->edge & 1) && (center->edge & 8)){ cellLookup->cells[cellLookup->found] 
                                                  = &DA_GRID[gridpos[1] + 1][gridpos[0] - 1];

                                                  cellLookup->found++;                                                  }
    if((center->edge & 2) && (center->edge & 8)){ cellLookup->cells[cellLookup->found]
                                                  = &DA_GRID[gridpos[1] + 1][gridpos[0] + 1];

                                                  cellLookup->found++;                                                  }
                                                                                                                        }

//  - --- - --- - --- - --- -

void DA_debug_drawgrid     (int pos[2])         {

    uint cx = pos[0] + DA_GRID_WIDTH  - (1 * (pos[0] > 0));
    uint cy = pos[1] + DA_GRID_HEIGHT - (1 * (pos[1] > 0));

    system("cls");
    for(int x = 0; x < DA_GRID_WIDTH * 2; x++) { printf("._"); } printf(".\n");

    for(int y = (DA_GRID_HEIGHT * 2) - 1; y > -1; y--)
    {
        for(int x = (DA_GRID_WIDTH * 2) - 1; x > -1; x--)
        {

            if( (x == cx) && (y == cy) ) { printf("|x"); }
            else                         { printf("|_"); }

        }

        printf("|\n");
    }

    for(int x = 0; x < DA_GRID_WIDTH * 2; x++) { printf("' "); } printf("'\n");                                         }

void DA_debug_drawgrid_sur (DACL* cellLookup)   {

    int* pos = cellLookup->cells[0]->worldpos;

    uint cx = pos[0] + DA_GRID_WIDTH  - (1 * (pos[0] > 0));
    uint cy = pos[1] + DA_GRID_HEIGHT - (1 * (pos[1] > 0));

    system("cls");
    for(int x = 0; x < DA_GRID_WIDTH * 2; x++) { printf("._"); } printf(".\n");

    for(int y = (DA_GRID_HEIGHT * 2) - 1; y > -1; y--)
    {
        for(int x = (DA_GRID_WIDTH * 2) - 1; x > -1; x--)
        {

            int is_sur = 0;
            for(uint i = 1; i < cellLookup->found; i++)
            {
                int* npos = cellLookup->cells[i]->worldpos;
                uint ncx  = npos[0] + DA_GRID_WIDTH  - (1 * (npos[0] > 0));
                uint ncy  = npos[1] + DA_GRID_HEIGHT - (1 * (npos[1] > 0));

                if( (x == ncx) && (y == ncy) ) { is_sur = 1; break; }

            }

            if     ( (x == cx) && (y == cy) )   { printf("|x"); }
            else if( is_sur                 )   { printf("|o"); }
            else                                { printf("|_"); }

        }

        printf("|\n");
    }

    for(int x = 0; x < DA_GRID_WIDTH * 2; x++) { printf("' "); } printf("'\n");                                         }

void DA_debug_drawgrid_cull(DACL* cellLookup)   {

    int* pos = cellLookup->cells[0]->worldpos;

    uint cx = pos[0] + DA_GRID_WIDTH  - (1 * (pos[0] > 0));
    uint cy = pos[1] + DA_GRID_HEIGHT - (1 * (pos[1] > 0));

    system("cls");
    for(int x = 0; x < DA_GRID_WIDTH * 2; x++) { printf("._"); } printf(".\n");

    for(int y = (DA_GRID_HEIGHT * 2) - 1; y > -1; y--)
    {
        for(int x = (DA_GRID_WIDTH * 2) - 1; x > -1; x--)
        {

            uint ncx      = x + DA_GRID_WIDTH  - (1 * (x > 0));
            uint ncy      = y + DA_GRID_HEIGHT - (1 * (y > 0));
            DA_CELL* cell = &DA_GRID[y][x];

            if     ( (x == cx) && (y == cy) )   { printf("|x"); }
            else if( cell->inFrustum        )   { printf("|o"); }
            else                                { printf("|_"); }

        }

        printf("|\n");
    }

    for(int x = 0; x < DA_GRID_WIDTH * 2; x++) { printf("' "); } printf("'\n");                                         }

//  - --- - --- - --- - --- -
