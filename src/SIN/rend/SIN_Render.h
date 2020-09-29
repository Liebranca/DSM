#ifndef __SIN_RENDER_H__
#define __SIN_RENDER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

void SIN_RenderBucket_init();
void SIN_RenderBucket_wipe();
void SIN_RenderBucket_end ();

//  - --- - --- - --- - --- -

extern int**  SIN_RenderBucket;
extern ushort SIN_RenderBucket_size;

#ifdef __cplusplus
}
#endif

#endif // __SIN_RENDER_H__
