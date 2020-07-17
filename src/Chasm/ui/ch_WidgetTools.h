#ifndef __CH_WIDGET_TOOLS_H__
#define __CH_WIDGET_TOOLS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../Zajec/lybyte/ZJC_BinTypes.h"

//  - --- - --- - --- - --- -

typedef struct CHASM_UI_WIDGET                  {

    float  position_x;
    float  position_y;

} chUW;

//  - --- - --- - --- - --- -

chUW* build_widget  (float position_x, float position_y, float* mesh, float* bounds);
int   del_widget    (chUW* widget);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __CH_WIDGET_TOOLS_H__