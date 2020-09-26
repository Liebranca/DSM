#ifndef __CH_JOY_H__
#define __CH_JOY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ZJC_CommonTypes.h"
#include "SDL/SDL.h"

//  - --- - --- - --- - --- -

#define CHJOYB_A           0x0001 //  0
#define CHJOYB_O           0x0002 //  1
#define CHJOYB_X           0x0004 //  2
#define CHJOYB_Z           0x0008 //  3

#define CHJOYB_R1          0x0080 //  7
#define CHJOYB_R2          0x0020 //  5
#define CHJOYB_R3          0x0800 // 11

#define CHJOYB_L1          0x0040 //  6
#define CHJOYB_L2          0x0010 //  4
#define CHJOYB_L3          0x0400 // 10

#define CHJOYB_SEL         0x0100 //  8
#define CHJOYB_STR         0x0200 //  9

#define CHJOYB_BUTTONMASK  0x001F
#define CHJOYB_TRIGGERMASK 0x0CF0

//  - --- - --- - --- - --- -

typedef struct CH_INPUT_JOYSTICK {

    fpair rgt_stick;
    fpair lft_stick;

    short buttons;
    float sens;

    int   instanceID;
    int   plugged;

} chJOY;

//  - --- - --- - --- - --- -

void chJOY_SETBUTTON      (chJOY* joy, uchar input);
void chJOY_CLRBUTTON      (chJOY* joy, uchar input);

int  chJOY_GB_AND         (chJOY* joy, short buttons);
int  chJOY_GB_OR          (chJOY* joy, short buttons);

int  chJOY_GB_XAND        (chJOY* joy, short buttons, short mask);

void chJOY_SETAXES        (chJOY* joy, SDL_JoyAxisEvent motion);

int  chJOY_RSTICK_ISACTI  (chJOY* joy);
int  chJOY_LSTICK_ISACTI  (chJOY* joy);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __CH_JOY_H__
