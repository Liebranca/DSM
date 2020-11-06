#ifndef __SIN_MATEBLOCK_H__
#define __SIN_MATEBLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ZJC_CommonTypes.h"
#define   SIN_MAX_MATERIALS             64

//  - --- - --- - --- - --- -

typedef struct SIN_MATERIAL_SHADERDATA {

    uint  flags;

    float spec_mult;
    float diff_mult;
    float ref_mult;
    float glow_rea;

} SIN_MATDATA;

typedef struct SIN_MATERIAL_UNIFORM {

    SIN_MATDATA materials[SIN_MAX_MATERIALS];

} SIN_UMATERIAL;

//  - --- - --- - --- - --- -

void         SIN_MateBlock_init       ();
void         SIN_MateBlock_end        ();
void         SIN_MateBlock_update     ();

void         SIN_MateBlock_setSlot    (uint slotnum, SIN_MATDATA* matdata);

void         SIN_MateBlock_requestUpdate();
void         SIN_MateBlock_updateCheck();
SIN_MATDATA* SIN_MateBlock_getSlot    (uint slotnum);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __SIN_MATEBLOCK_H__
