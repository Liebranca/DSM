#include "SIN.H"

#include "types/SIN_Mesh.h"
#include "types/SIN_Material.h"

#include <stdio.h>

unsigned char INITFLAGS = 0;

void SIN_INIT(unsigned char flags)              { 

    SIN_matbucket_init();

    if(flags & 1)                               { SIN_meshbucket_init3d();                                              }

    INITFLAGS = flags;

                                                                                                                        }

void SIN_END()                                  { 

    SIN_matbucket_end();

    if(INITFLAGS & 1)                           { SIN_meshbucket_end3d(); printf("ASSHOLE");                            }

                                                                                                                        }

