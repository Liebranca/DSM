#ifndef __ESPECTRO_H__
#define __ESPECTRO_H__

#include "ZJC_CommonTypes.h"
#include "types/SPCTR_BaseHED.h"

#define EXPORT __declspec(dllexport)

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
extern "C" {
#endif

// Init for the inner cogs in ESPECTRO
EXPORT void ESPECTRO_INIT(uint num_conts, uint contsize);

// Clean-up for ESPECTRO
EXPORT void ESPECTRO_END();

// Assigns values to a block within SPCTR_CONTS
EXPORT uint SPCTR_fill_gBlock(HED* hed, uint id);

// Frees a block within SPCTR_CONTS for later use
EXPORT void SPCTR_del_gBlock(uint id);

// Get handle to a block from it's ID
EXPORT HED** SPCTR_find_gBlock(uint id);

EXPORT void shohed();

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ESPECTRO_H__
