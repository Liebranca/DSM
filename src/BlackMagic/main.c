#include <stdio.h>

#include "lyutils/ZJC_Evil.h"
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

        if     (converter == "crk")             { writecrk(filename, archive, writemode, writeoffset);                  }
        else if(converter == "joj")             { writejoj(filename, archive, writemode, writeoffset);                  }

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
