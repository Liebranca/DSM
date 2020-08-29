#ifndef __DARKAGE_COMMONTYPES_H__
#define __DARKAGE_COMMONTYPES_H__

#include "ZJC_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

typedef struct DA_NODE_CELLINFO {

    uint index;
    uint gridpos [2];
    int  worldpos[2];
} DANCI;

typedef struct DA_GRIDCELL_INFO {

    uint gridpos[2];
    int  worldpos[2];
} DAGCI;

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __DARKAGE_COMMONTYPES_H__

