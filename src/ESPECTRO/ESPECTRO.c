#include "ESPECTRO.h"

#include "lyarr/ZJC_Container.h"
#include "lymath/ZJC_GOPS.h"

#include "lyutils/ZJC_Evil.h"

#include "types/SPCTR_Generic.h"

#include <stdio.h>

//  ---     ---     ---     ---     ---

static uint        SPCTR_NUMCONTS       = 10;   // Number of containers to create on init: defaults to 10
static cuint       SPCTR_MAXCONTS       = 64;   // Maximum number of containers

static uint        SPCTR_CONTSIZE       = 1024; // Number of elements per container: defaults to 1024
static Container** SPCTR_CONTS          = NULL; // An array of containers holding HED blocks

static Stack*      SPCTR_gblock_idStack = NULL; // A stack holding unique numerical ids for blocks
static uint        SPCTR_active_conts   = 0;    // How many containers are actually in use
static uint        SPCTR_MAXID          = 0;    // Maximum number of block ids (container size * count)

//  ---     ---     ---     ---     ---

EXPORT void ESPECTRO_INIT(uint num_conts,
                          uint contsize )       {

    printf("called init\n");

    WARD_EVIL_MALLOC                            (SPCTR_CONTS, Container*, sizeof(Container*), SPCTR_NUMCONTS            );
    SPCTR_NUMCONTS = clampi                     (num_conts, 1, SPCTR_MAXCONTS                                           );

    for(uint i = 0; i < SPCTR_NUMCONTS; i++)
    {   SPCTR_CONTS[i]   = ZJC_build_cont       (SPCTR_CONTSIZE, sizeof(HED*), SPCTR_CONTSIZE, "GameValue"              );
                                                                                                                        }

    SPCTR_MAXID = 1 + (SPCTR_NUMCONTS * SPCTR_CONTSIZE);
    SPCTR_gblock_idStack = ZJC_build_stack      (SPCTR_MAXID                                                            );
    for(uint i = SPCTR_MAXID; i > 0; i--)       { ZJC_push_stack(SPCTR_gblock_idStack, i);                              }
                                                                                                                        }

EXPORT void ESPECTRO_END()                      { ZJC_del_stack(SPCTR_gblock_idStack); printf("called destructor\n");
    for(uint i = 0; i < SPCTR_NUMCONTS; i++)    { ZJC_del_cont (SPCTR_CONTS[i]      ); } WARD_EVIL_MFREE(SPCTR_CONTS);  }

//  ---     ---     ---     ---     ---
// Return index of container holding this id
uint SPCTR_contIdex_fromID(uint id)             { return id / (SPCTR_CONTSIZE + 1);                                     };

// Return address of block with this id
EXPORT HED** SPCTR_find_gBlock(uint id)         { uint cont_idex = SPCTR_contIdex_fromID(id);
                                                  ZJC_findItem_cont(SPCTR_CONTS[cont_idex], HED*, id, 0);               }

//  ---     ---     ---     ---     ---

EXPORT uint SPCTR_fill_gBlock(HED* hed,
                              uint id)          {

    uint  loc;                                  // location of block
    uint  cont_idex;                            // index of container that holds block
    HED** block;                                // address of block to modify

//  ---     ---     ---     ---     ---
// when assigning values to a free block
    if (!id)                                    {

        id         = ZJC_pop_stack              ( SPCTR_gblock_idStack                            );
        if(!id)                                 { printf("All containers are full!\n"); return 0; }
                     WARD_EVIL_UNSIG            ( id,                     1                       );

        cont_idex  = SPCTR_contIdex_fromID      ( id                                              );

        loc        = ZJC_push_cont              ( SPCTR_CONTS[cont_idex], id                      );
                     WARD_EVIL_UNSIG            ( loc,                     1                      );

        block      =                            ( (HED**) SPCTR_CONTS[cont_idex]->buff            ) + loc;

//  ---     ---     ---     ---     ---
// when updating a block that's in use
    } else                                      { block = SPCTR_find_gBlock(id);                  }

// assign values. always return id of modified block
    *block  =  hed;
    printf("%p %p\n", block, *block);

 return id;                                                                                            }

//  ---     ---     ---     ---     ---

EXPORT void SPCTR_del_gBlock(uint id)           {

    uint cont_idex = SPCTR_contIdex_fromID      (id                           );
    uint loc       = ZJC_findLoc_cont           (SPCTR_CONTS[cont_idex], id, 0);

/* TODO: memfree here if hed->value points to dynamic memory */

    ZJC_push_stack                              (SPCTR_gblock_idStack,   id   );
    ZJC_pop_cont                                (SPCTR_CONTS[cont_idex], id   );
                                                                                                                        }

//  ---     ---     ---     ---     ---

EXPORT void shohed() {

    printf("\n");
    HED* hed = *SPCTR_find_gBlock(1);

    fuck();

    //IBAR* v = KLS_CAST(01, hed->value);
    printf("(%s) %s: %s\n", SPCTR_getcurr_KLS(&hed->kls), hed->name, hed->description);
}

//  ---     ---     ---     ---     ---
