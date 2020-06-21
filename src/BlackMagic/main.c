#include <stdio.h>

#include "lyutils/ZJC_Evil.h"
#include "lybyte/ZJC_FileHandling.h"

#include "../SIN/SIN.h"
#include "wind/chMANG.h"

//  - --- - --- - --- - --- -

int main(int argc, char* argv[])
{

    chmang_init("BlackMagic", 640, 480);
    SIN_INIT(SIN_INIT_3D);

    while(chmang_winOpen())
    {

        chmang_frameStart();
        chmang_wait();
        chmang_frameEnd();
    }

    SIN_END();
    chmang_end();
    return 0;
}
