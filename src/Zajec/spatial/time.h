#ifndef TIME_H
#define TIME_H

//  - --- - --- - --- - --- -

#include <chrono>
#include <iostream>
#include <string>
#define ch std::chrono

//  - --- - --- - --- - --- -

namespace timeops {
    // Gets elapsed time each frame.
    class DS_TIME {
    public:
        DS_TIME() { calcFrametime(); };
        virtual ~DS_TIME() {};

        // Get clock time at start of frame
        void frameStart() { begin = ch::steady_clock::now(); calcWorkTime(); }
        
        // Get clock time at end of frame
        void frameEnd() { end = ch::steady_clock::now(); calcSleepTime(); calcTotal(); }

        // Compute elapsed frametime in millis * timescale
        float calcCurr();

        bool getSleep() { return (frametime > worktime.count()); }
        
        double getDelta() { 
            ch::duration<double, std::milli> delta_ms(frametime - worktime.count());
            auto delta_ms_duration = ch::duration_cast<ch::milliseconds>(delta_ms);
            return (double)delta_ms_duration.count();
        }

//      - --- - --- - --- - --- -

        // Multiplies float value by elapsed time.
        //* Used mainly to keep speed/acceleration values relative to realtime.
        float fBy(float f) { return f * curr; }

        // Get total elapsed time since simulation start.
        float calcTotal() { total += calcCurr(); return total; }

        // Prints total to console.
        void getRead();

        // Gets current fraction of realtime elapsed for this frame.
        float getCurr() { return curr; }

        // Get time multiplier
        double getScale() { return scale; }

        // Sets the time multiplier
        void setScale(float newScale) { scale = newScale; }

        // Divide 1 second by framecap.
        void calcFrametime() { frametime = 1000.0 / framecap; }

        // Sets framecap to new value. Beware!
        void setFramecap(int newcap) { framecap = newcap; calcFrametime(); }

//      - --- - --- - --- - --- -

    private:
        ch::steady_clock::time_point begin, end;
        ch::duration<double, std::milli> worktime, sleeptime;
        
        int framecap = 60; double frametime;

        // Framecap helper
        void calcSleepTime() { sleeptime = end - begin; }

        // Framecap helper
        void calcWorkTime() { sleeptime = begin - end; }

        float total = 0.0f, curr = 0.0f, scale = 1.0f;

    };

//  - --- - --- - --- - --- -

    // Simple timer class, mainly used to drive animations.
    class DS_TICKER {

    public:
        DS_TICKER(float max) { this->max = max; this->curr = max; }

        virtual ~DS_TICKER() { ; }
        float curr = 0.0f, max = 0.0f;
        int cycles = -1;

//      - --- - --- - --- - --- -

        // Returns total elapsed time since last restart.
        float getp() { return (this->curr / this->max) + this->cycles; }

        // Restarts the timer
        void restart() { this->curr = 0.0f; this->cycles = -1; }

        // Advances the timer by elapsed. If curr >= max, move up a cycle.
        bool update(float elapsed);

    };
}

namespace lyspace {
    using namespace timeops;
}

#endif //TIME_H
