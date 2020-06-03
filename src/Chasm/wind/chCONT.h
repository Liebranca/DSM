#ifndef CH_CONT_H
#define CH_CONT_H

//  - --- - --- - --- - --- -

#include "SDL/SDL.h"
#include "spatial/time.h"

//  - --- - --- - --- - --- -

// Core opengl manager
class chCONT {

    public:
        chCONT();
        ~chCONT();

        bool winOpen();
        void pollEvents();

        void chkwait();

//  - --- - --- - --- - --- -

    private:
        SDL_GLContext context;
        lyspace::DS_TIME* time_internal;

};

//  - --- - --- - --- - --- -

#endif // CH_CONT_H
