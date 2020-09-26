#include "chJOY.h"

#include <stdio.h>

//  - --- - --- - --- - --- -

cint        CH_NUM_BUTTONS  = 12;

const float joyDead         = 0.030f;
const float joyDiv          = 1.0f/32768.0f;

//  - --- - --- - --- - --- -

void chJOY_SETBUTTON (chJOY* joy,
                      uchar input     )         { joy->buttons |=  (1 << input);                                        }

void chJOY_CLRBUTTON (chJOY* joy,
                      uchar input     )         { joy->buttons &= ~(1 << input);                                        }

int  chJOY_GB_AND    (chJOY* joy,
                      short buttons   )         { return (buttons & joy->buttons) == buttons;                           }

int  chJOY_GB_OR     (chJOY* joy,
                      short buttons   )         { return (buttons & joy->buttons) != 0;                                 }

int  chJOY_GB_XAND   (chJOY* joy,
                      short buttons,
                      short mask      )         { return buttons == (joy->buttons & mask);                              }

//  - --- - --- - --- - --- -

void chJOY_SETAXES   (chJOY* joy,
                      SDL_JoyAxisEvent motion)  {

    if      (motion.axis == 0)                  { joy->lft_stick.x = motion.value * joyDiv * joy->sens;                 }
    else if (motion.axis == 1)                  { joy->lft_stick.y = motion.value * joyDiv * joy->sens;                 }
    else if (motion.axis == 2)                  { joy->rgt_stick.y = motion.value * joyDiv;                             }
    else if (motion.axis == 3)                  { joy->rgt_stick.x = motion.value * joyDiv;                             }

                                                                                                                        }

int  chJOY_RSTICK_ISACTI(chJOY* joy)            { return    fabs(joy->rgt_stick.x) > joyDead
                                                         || fabs(joy->rgt_stick.y) > joyDead;                           }

int  chJOY_LSTICK_ISACTI(chJOY* joy)            { return    fabs(joy->lft_stick.x) > joyDead
                                                         || fabs(joy->lft_stick.y) > joyDead;                           }

//  - --- - --- - --- - --- -
