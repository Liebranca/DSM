/*═════════════════════════════════════════════════════════════════════════╗
║ i created a separate project because you schmucks made it so compiling   ║
║ a single file to test a random function is way too complicated           ║
║ like fucking hell, i could've gotten this done quicker in turbo c        ║
║                                                                          ║
║ and i would normally add these test files to gitignore, but fuck you     ║
║ im leaving this here as testament to your incompetence                   ║
╚═════════════════════════════════════════════════════════════════════════*/

#include <stdio.h>
#include "lymath/ZJC_Vec.h"

int main()
{
    fvec2 vec0     = {  0,  0       };
    fvec2 vec1     = { 15, .152f    };

    fvec2* vecs[]  = { &vec0, &vec1 };

    ZJC_load_fvec2(vecs, 2, 0); ZJC_setscp_fvec2(0, 2);
    printf("%p, %p\n", &vec0, &vec1);
    printf("%p, %p | CURPOS %u\n\n", ZJC_getp_fvec2(0, 0), ZJC_getp_fvec2(1, 1), ZJC_getpos_fvec2());

    printf("[%f, %f] || [%f, %f]\n\n", vec0.x, vec0.y, vec1.x, vec1.y);

    printf("Are 0 and 1 Equal? %i\n", ZJC_equals_fvec2());
    printf("Is 1 zero? %i\n", ZJC_zcheck_fvec2());
    ZJC_setpos_fvec2(0);
    printf("Is 0 zero? %i\n", ZJC_zcheck_fvec2());

    ZJC_setpos_fvec2(1);
    ZJC_copy_fvec2(1); printf("\nCOPYLEFT! [%f, %f] <- [%f, %f]\n\n", vec0.x, vec0.y, vec1.x, vec1.y);
    printf("Are 0 and 1 Equal? %i\n", ZJC_equals_fvec2());

    ZJC_setpos_fvec2(0); printf("CURPOS: %u\n\n", ZJC_getpos_fvec2());

    ZJC_setscp_fvec2(0, 1);
    ZJC_fill_fvec2(1); printf("\nFILL 0 WITH 1s! [%f, %f] || [%f, %f]\n", vec0.x, vec0.y, vec1.x, vec1.y);
    ZJC_setscp_fvec2(0, 2);
    ZJC_copy_fvec2(0); printf("COPYRIGHT! [%f, %f] -> [%f, %f]\n\n", vec0.x, vec0.y, vec1.x, vec1.y);

    printf("Are 0 and 1 Equal? %i\n", ZJC_equals_fvec2());
    printf("Is 0 zero? %i\n", ZJC_zcheck_fvec2());
    ZJC_setpos_fvec2(1);
    printf("Is 1 zero? %i\n", ZJC_zcheck_fvec2());

    ZJC_fill_fvec2(2); printf("\nFILL WITH 2s! [%f, %f] -> [%f, %f]\n", vec0.x, vec0.y, vec1.x, vec1.y);

    ZJC_setpos_fvec2(0);
    printf("Are 0 and 1 Equal? %i\n", ZJC_equals_fvec2());
    printf("Is 0 zero? %i\n", ZJC_zcheck_fvec2());
    ZJC_setpos_fvec2(1);
    printf("Is 1 zero? %i\n", ZJC_zcheck_fvec2());

    ZJC_flush_fvec2();
    printf("\n");

}

