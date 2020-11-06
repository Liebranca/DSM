#ifndef __SIN_LIGHTING_H__
#define __SIN_LIGHTING_H__

#include "glm/glm.hpp"
#include "ZJC_CommonTypes.h"
#include "../SIN_Constants.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

struct SIN_LAMP {

    glm::mat4  mat;

    glm::vec4  position;        // posx, posy, posz, rea
    glm::vec4  color;           // colr, colg, colb, intensity
    glm::vec4  dir;             // dirx, diry, dirz, 0
};

struct SIN_ULIGHTS {

    SIN_LAMP lamps[SIN_MAX_LAMPS];
    uint     num_lamps;
};

//  - --- - --- - --- - --- -

void SIN_Lighting_init       ();
void SIN_Lighting_end        ();

void SIN_Lighting_setDir     (SIN_LAMP* lamp, uchar lamp_index);
void SIN_Lighting_setNum     (uint num_lamps);
void SIN_Lighting_updateCheck();

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __SIN_LIGHTING_H__
