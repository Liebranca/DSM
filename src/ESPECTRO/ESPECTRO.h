#ifndef __ESPECTRO_H__
#define __ESPECTRO_H__

#define EXPORT __declspec(dllexport)

#include "ZJC_CommonTypes.h"

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
extern "C" {
#endif

//  ---     ---     ---     ---     ---
// just a dummy func so i can test out ctypes python-side
EXPORT cuchar* dooku();

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ESPECTRO_H__
