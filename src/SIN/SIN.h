#ifndef __SIN_H__
#define __SIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "GL/glew.h"

#define SIN_INIT_3D 0x01

void SIN_INIT(unsigned char flags);
void SIN_END ();

#ifdef __cplusplus
}
#endif

#endif // __SIN_H__
