#ifndef WORLDMAN_H
#define WORLDMAN_H

#include "node.h"
#include "../backbone/reyn.h"

namespace meWorld
{
	static void rugPull() {
		reyn->rugPulled = false;
		int dirnx = -1, dirnz = -1;
		bool treshx = fabs(og->worldPosition().x) > 12, treshz = fabs(og->worldPosition().z) > 12;
		if (og->worldPosition().x < 0) { dirnx = 1; } if (og->worldPosition().z < 0) { dirnz = 1; }
		if(treshx || treshz) {
			glm::vec3 mvec = { treshx * 24 * dirnx, 0, treshz * 24 * dirnz };
			reyn->worldCoor += glm::vec2(treshx * -dirnx, treshz * -dirnz);
			reyn->rugPulled = true; reyn->rugVec = mvec;
			std::cout << "WC: " << reyn->worldCoor.x << ", " << reyn->worldCoor.y << std::endl;
		}

	}

};

#endif //WORLDMAN_H
