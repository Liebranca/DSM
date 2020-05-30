#ifndef LIGHT_H
#define LIGHT_H

#include <vector>
#include "../lymath/transform.h"

class DS_LIGHT {

	public:
		DS_LIGHT(glm::vec3 position = { 0,0,0 }, glm::vec4 color = { 1,1,1,1 }, float radius = 1.0f) {
			this->m = new DS_TRANSFORM(position);
			this->color = color; NUM_LIGHTS++;
			this->radius = radius;
		}
		
		virtual ~DS_LIGHT() { ; }

		DS_TRANSFORM* getTransform() { return this->m; }
		void setParent(DS_TRANSFORM* parent) { this->m->setParent(parent); }
		inline glm::vec3& worldPosition() { return this->m->worldPosition(); }
		inline glm::vec4& getCol() { return this->color; }
		inline glm::vec4 shedPosition() {
			return glm::vec4(this->m->worldPosition(), 1) * this->m->getModel();
		}

		float radius, oldradius; bool obstructed = false;
		void obstruct() {
			if (!this->obstructed) {
				this->oldradius = this->radius;
				this->obstructed = true;
			}
			else if (this->radius > this->oldradius / 2) {
				this->radius -= 0.0025f;
			}
		}

		void restore() {
			
			if (this->radius < this->oldradius) {
				this->radius += 0.001f;
			}
			else if (this->obstructed) {
				this->obstructed = false;
				this->radius = this->oldradius;
			}
		}

		bool novCheck(int x, int z) {
			bool novchange = (this->currnov[0] != x || this->currnov[1] != z);
			return novchange;
		}

		bool ennCheck(int x, int z) {
			bool ennchange = (this->currenn[0] != x || this->currenn[1] != z);
			return ennchange;
		}

		std::vector<int> onNovChange(int x, int z) {
			std::vector<int> oldnov = { this->currnov[0], this->currnov[1] };
			this->setCurrnov(x, z); return oldnov;
		}

		std::vector<int> onEnnChange(int x, int z) {
			std::vector<int> oldenn = { this->currenn[0], this->currenn[1] };
			this->setCurrenn(x, z); return oldenn;
		}
		
	private:
		DS_TRANSFORM* m = nullptr;
		glm::vec4 color;

		int currnov[2] = { 0,0 };
		int currenn[2] = { 0,0 };
		void setCurrnov(int x, int z) { this->currnov[0] = x; this->currnov[1] = z; }
		void setCurrenn(int x, int z) { this->currenn[0] = x; this->currenn[1] = z; }

};

extern std::vector<DS_LIGHT*> Lights;
extern std::vector<DS_LIGHT*> nearbyLights;

#endif //LIGHT_H
