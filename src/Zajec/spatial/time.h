#ifndef TIME_H
#define TIME_H

//	- --- - --- - --- - --- -

#include <chrono>
#define ch std::chrono

//	- --- - --- - --- - --- -

namespace timeops {
	// Gets elapsed time each frame.
	class DS_TIME {
	public:
		DS_TIME() {};
		virtual ~DS_TIME() {};

		// Get clock time at start of frame
		void frameStart() { begin = ch::steady_clock::now(); }

		// Get clock time at end of frame
		void frameEnd() { end = ch::steady_clock::now(); calcTotal(); }

		// Get elapsed time from frame based on (end - begin) / 3.14f
		float calcCurr();

		//		- --- - --- - --- - --- -

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

		//		- --- - --- - --- - --- -

	private:
		ch::steady_clock::time_point begin, end;
		float total = 0.0f, curr = 0.0f;
		float scale = 1.0f;

	};

	//	- --- - --- - --- - --- -

		// Simple timer class, mainly used to drive animations.
	class DS_TICKER {

	public:
		DS_TICKER(float max) { this->max = max; this->curr = max; }

		virtual ~DS_TICKER() { ; }
		float curr = 0.0f, max = 0.0f;
		int cycles = -1;

		//		- --- - --- - --- - --- -

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
