#include "DA_WorldManager.h"

#include "lyutils/ZJC_Evil.h"
#include "lymath/ZJC_GOPS.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static int      DA_GRID_HEIGHT      = 2;
static int      DA_GRID_WIDTH       = 2;

static cuint     DA_CELL_SIZE       = 8;

static int       DA_WORLD_OFFSET[2] = { 0, 0 };

const float      DA_CELL_DIV        = 1.0f/DA_CELL_SIZE;
const float      DA_CELL_QUAT       = DA_CELL_SIZE * 0.25f;

static DA_CELL** DA_GRID            = NULL;

//  - --- - --- - --- - --- -

void DA_grid_makecell(DA_CELL** cell,
                      int x, int y)             {

    uint go_right        = 1 * (x > -(DA_GRID_WIDTH));
    uint go_left         = 2 * (x <  (DA_GRID_WIDTH));
    uint go_down         = 4 * (y > -(DA_GRID_HEIGHT));
    uint go_up           = 8 * (y <  (DA_GRID_HEIGHT));

    (*cell)->worldpos[0] = x;
    (*cell)->worldpos[1] = y;

    (*cell)->edge        = (go_right | go_left | go_down | go_up);                                                      }

void DA_grid_end ()                             { for(int i = 0; i < DA_GRID_HEIGHT * 2; i++)
                                                { WARD_EVIL_MFREE(DA_GRID[i]);  } WARD_EVIL_MFREE(DA_GRID);             }

void DA_grid_init(int width, int height)        {

    DA_GRID_WIDTH  = width;
    DA_GRID_HEIGHT = height;

    if(DA_GRID != NULL) { DA_grid_end(); }

    DA_GRID = (DA_CELL**) evil_malloc(height * 2, sizeof(DA_CELL*));
    for(int i = 0; i < height * 2; i++)             { DA_GRID[i] = (DA_CELL*)evil_malloc(width * 2, sizeof(DA_CELL));   }

    for(int y = 0; y < height * 2; y++)
    {
        for(int x = 0; x < width * 2; x++)
        {

            int wx  = x - width;
            int wy  = y - height;
            if(wx >= 0) { wx++; }
            if(wy >= 0) { wy++; }

            DA_CELL* cell = &DA_GRID[y][x];
            DA_grid_makecell(&cell, wx,  wy);

        }
    }
                                                                                                                        }

//  - --- - --- - --- - --- -

void DA_grid_modWorldOffset(int mvec[2])        {

    DA_WORLD_OFFSET[0] += mvec[0];
    DA_WORLD_OFFSET[1] += mvec[1];                                                                                      }

void DA_grid_calcRugPull   (int mvec[2])        {

    if(mvec[0])                                 { mvec[0] = mvec[0] * DA_CELL_SIZE;                                     }
    if(mvec[1])                                 { mvec[1] = mvec[1] * DA_CELL_SIZE;                                     }
                                                                                                                        }

//  - --- - --- - --- - --- -

void DA_grid_regObject     (int    pos[2],
                            uint   ipos[3],
                            ushort obloc,
                            int    is_dynamic)  {

    ipos[1] = pos[0] + DA_GRID_WIDTH  - (1 * (pos[0] > 0));
    ipos[2] = pos[1] + DA_GRID_HEIGHT - (1 * (pos[1] > 0));

    DA_CELL* cell = &DA_GRID[ipos[1]][ipos[2]];

    if(is_dynamic)  { ipos[0] = cell->num_dynamics + (__DA_MAX_OBJECTS_PER_CELL__ / 2);

                      cell->oblocs[ipos[0]] = obloc;
                      cell->num_dynamics++;                                                                             }

    else            { ipos[0] = cell->num_statics;

                      cell->oblocs[ipos[0]] = obloc;
                      cell->num_statics++;                                                                              }
                                                                                                                        }

void DA_grid_unregObject   (uint ipos[3],
                            int   is_dynamic) {

    DA_CELL* cell = &DA_GRID[ipos[1]][ipos[2]];

    uint last_index;

    if(is_dynamic) { last_index = (cell->num_dynamics + (__DA_MAX_OBJECTS_PER_CELL__ / 2)) - 1;
                     cell->num_dynamics--;                                                      }

    else           { last_index = cell->num_statics; cell->num_statics--;                       }

    cell->oblocs[ipos[0]]    = cell->oblocs[last_index];
    ipos[1]                  = cell->oblocs[ipos[0]];

    cell->oblocs[last_index] = 0;                                                                                       }

//  - --- - --- - --- - --- -

void DA_grid_findpos      (int   dest[2],
                           float pos[3])        {

    dest[0] = (int)truncf(pos[0] * DA_CELL_DIV) + flipifi(1, pos[0] < 0);
    dest[1] = (int)truncf(pos[2] * DA_CELL_DIV) + flipifi(1, pos[2] < 0);

    dest[0] = clampi(dest[0] + DA_WORLD_OFFSET[0], -DA_GRID_WIDTH,  DA_GRID_WIDTH );
    dest[1] = clampi(dest[1] + DA_WORLD_OFFSET[1], -DA_GRID_HEIGHT, DA_GRID_HEIGHT);

    if(dest[0] == 0) { dest[0] +=  flipifi(DA_WORLD_OFFSET[0] != 0, DA_WORLD_OFFSET[0] < 0); }
    if(dest[1] == 0) { dest[1] +=  flipifi(DA_WORLD_OFFSET[1] != 0, DA_WORLD_OFFSET[1] < 0); }

}

void DA_grid_getsurround  (DACL* cellLookup,
                           int   pos[2])        {

    uint x               = pos[0] + DA_GRID_WIDTH  - (1 * (pos[0] > 0));
    uint y               = pos[1] + DA_GRID_HEIGHT - (1 * (pos[1] > 0));

    DA_CELL center       =  DA_GRID[y][x];

    cellLookup->cells[0] = &DA_GRID[y][x];
    cellLookup->found    = 1;

    if( center.edge & 1 )                       { cellLookup->cells[cellLookup->found] = &DA_GRID[y][x - 1];
                                                  cellLookup->found++;                                                  }
    if( center.edge & 2 )                       { cellLookup->cells[cellLookup->found] = &DA_GRID[y][x + 1];
                                                  cellLookup->found++;                                                  }
    if( center.edge & 4 )                       { cellLookup->cells[cellLookup->found] = &DA_GRID[y - 1][x];
                                                  cellLookup->found++;                                                  }
    if( center.edge & 8 )                       { cellLookup->cells[cellLookup->found] = &DA_GRID[y + 1][x];
                                                  cellLookup->found++;                                                  }

    if((center.edge & 1) && (center.edge & 4))  { cellLookup->cells[cellLookup->found] = &DA_GRID[y - 1][x - 1];
                                                  cellLookup->found++;                                                  }
    if((center.edge & 2) && (center.edge & 4))  { cellLookup->cells[cellLookup->found] = &DA_GRID[y - 1][x + 1];
                                                  cellLookup->found++;                                                  }
    if((center.edge & 1) && (center.edge & 8))  { cellLookup->cells[cellLookup->found] = &DA_GRID[y + 1][x - 1];
                                                  cellLookup->found++;                                                  }
    if((center.edge & 2) && (center.edge & 8))  { cellLookup->cells[cellLookup->found] = &DA_GRID[y + 1][x + 1];
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

//  - --- - --- - --- - --- -
