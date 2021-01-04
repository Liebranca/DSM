#include <stdio.h>
#include <string.h>

#include "lyutils/ZJC_Evil.h"
#include "lyutils/ZJC_Str.h"
#include "lybyte/ZJC_FileHandling.h"

float background_color[4] = { 0.75f, 0.35f, 0.0f, 1.0f };

cuint  winwidth = 640;
cuint  winheight = 480;

//  - --- - --- - --- - --- -

int main(int argc, char* argv[])
{

    if (argc == 6) 
    {
        char* converter    = argv[1];
        char* filename     = argv[2];
        char* archive      = argv[3];
        char* writemode    = argv[4];
        char* writeoffset  = argv[5];

        if     (!strcmp(converter, "crk"))      { writecrk(filename, archive, writemode, writeoffset);

    #if 0
    // TESTBLOCK |
    //           v
    /**/    ZBIN daf    =                       { archive,  NULL                                                        };
    /**/    ZJC_open_daf                        ( &daf,     0                                                           );
    /**/
    /**/    char* strgname; catpath             ( archive,  "STRG", &strgname                                           );
    /**/    ZBIN strg   =                       { strgname, NULL                                                        };
    /**/
    /**/    ZJC_init_crk                        (                                                                       );
    /**/
    /**/    ZJC_extract_crk                     ( &daf,     &strg,  0, 1                                                );
    /**/    ZJC_read_crkframe                   ( &strg                                                                 );
    /**/
    /**/    MD3D* mdata = ZJC_get_crkdata       (                                                                       );
    /**/    uint co     = mdata->verts[0].frac1 & 0x00FFFFFF;
    /**/
    /**/    float x     = frac_tofloat          (  co & 0x0000FF,        256, FRAC5, 128                                );
    /**/    float y     = frac_tofloat          ( (co & 0x00FF00) >>  8, 256, FRAC5, 128                                );
    /**/    float z     = frac_tofloat          ( (co & 0xFF0000) >> 16, 256, FRAC5, 128                                );
    /**/
    /**/    printf                              ( "[%f, %f, %f]\n", x, y, z                                               );
    /**/
    /**/    ZJC_close_daf                       ( &daf, 1                                                               );
    /**/    ZJC_end_crk                         (                                                                       );
    /**/
    /**/    closebin                            ( &strg, 1                                                              );
    /**/    WARD_EVIL_MFREE                     ( strgname                                                              );
    //           ^
    // TESTBLOCK |
    #endif
                                                                                                                        }
        else if(!strcmp(converter, "joj"))      { writejoj(filename, archive, writemode, writeoffset);                  }

    }

    else
    {
        printf("Usage: <converter> <filepath> <archive> <writemode> <writeoffset>\n");
        printf("\n-Converter:\n    crk: for 3D meshes\n    joj: for images\n");
        printf("\n-Filepath: absolute path to a hexdump. Deleted if succesfully converted\n");
        printf("\n-Archive: absolute path to a DAF file. New is created if file doesn't exist\n");
        printf("\n-Writemode:\n    0x00: create new\n    0x01: append to existing\n    0x02: update existing\n");
        printf("\n-Writeoffset (0x00 to 0xFF): position of item in DAF. Only used for update\n");

        printf("\nPress any key to exit...");
        getchar();
    }

    return 0;
}
