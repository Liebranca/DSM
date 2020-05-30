#ifndef TIME_H
#define TIME_H

#include <chrono>
#include <string>
#include "GL/glew.h"

#define ch std::chrono

class DS_TIME {
	public:
		DS_TIME() {};
		virtual ~DS_TIME() {};
		
		void frameStart() { begin = ch::steady_clock::now(); }
		void frameEnd() { end = ch::steady_clock::now(); calcTotal(); }
		float calcCurr() {
			curr = (ch::duration_cast< ch::duration<float> >(end - begin).count() / 3.14f ) * scale;
			return curr; }

		float fBy(float f) { return f * curr; }
		
		float calcTotal() { total += calcCurr(); return total; }
		std::string getRead();

		float getCurr() { return curr; }
		double getScale() { return scale; }
		void setScale(float newScale) { scale = newScale; }

	private:
		ch::steady_clock::time_point begin, end;
		float total = 0.0f, curr = 0.0f;
		float scale = 1.0f;

};

class DS_TICKER {
	
	public:
		DS_TICKER(float max) {
			this->max = max;
			this->curr = max;
		}

		virtual ~DS_TICKER() { ; }
		float curr = 0.0f, max = 0.0f;
		int cycles = -1;

		float getp(){ return (this->curr / this->max) + this->cycles; }

		bool update(float elapsed) {
			this->curr += elapsed;
			if (this->curr >= max) { this->curr = 0.0f; this->cycles++; return true; }
			return false;
		}

		void restart() { this->curr = 0.0f; this->cycles = -1; }

};

#endif //TIME_H
