#include "lyutils/ZJC_Evil.h"
#include "lybyte/ZJC_FileHandling.h"

#include "../SIN/SIN.h"

#include "wind/chMANG.h"

//  - --- - --- - --- - --- -

int main(int argc, char* argv[])
{

    /*if(argc != 5) { fprintf(stderr,
        "usage: %s <filename> <archive> offset mode ", argv[0]); exit(EXIT_FAILURE); }

    int errorcode = 0;
    WARD_EVIL_WRAP(errorcode, writecrk(argv[1], argv[2], argv[3], argv[4]));*/

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
