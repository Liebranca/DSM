#include "SIN.H"

#include "types/SIN_Mesh.h"
#include "types/SIN_Material.h"

unsigned char INITFLAGS = 0;

void SIN_INIT(unsigned char flags)              { 

    SIN_matbucket_init();
    SIN_meshbucket_init();

    INITFLAGS = flags;

                                                                                                                        }

void SIN_END()                                  { 

    SIN_matbucket_end();
    SIN_meshbucket_end();
                                                                                                                        }

