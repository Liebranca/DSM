#ifndef __SPCTR_NUMERICS_H__
#define __SPCTR_NUMERICS_H__

#include "ZJC_CommonTypes.h"

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ESPECTRO_IBAR {

    sshort max;
    sshort min;
    sshort cur;
    sshort mod;

} IBAR;

//  ---     ---     ---     ---     ---

IBAR* SPCTR_build_IBAR(sshort max, sshort min, sshort cur, sshort mod);
void  SPCTR_del_IBAR(IBAR* bar);

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SPCTR_NUMERICS_H__
