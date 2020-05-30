#ifndef SPAWNMAN_H
#define SPAWNMAN_H

#include <string>
#include "node.h"
#include "../backbone/space.h"

static DS_NODE* spawnOnce(std::string filename,
	glm::vec3 at = glm::vec3(0,0,0), glm::quat rot = glm::quat(1,0,0,0),
	glm::vec3 scale = glm::vec3(1,1,1) ) {
	DS_NODE* other = nullptr;
	DS_NODE* newObject = new DS_NODE(filename);
	newObject->worldPosition() += at;

	std::vector<std::vector<DS_NODE*>> nov_oblist;
	nov_oblist = ct->locateObject(newObject);

	if(newObject->getPhysMode() == 2) {
		for (unsigned int novi = 0; novi < nov_oblist.size(); novi++) {
			for (unsigned int obm = 0; obm < nov_oblist[novi].size(); obm++) {
				other = nov_oblist[novi][obm];
				if (newObject != other) {
					if (other->getIsMapNode() && newObject->sharesSpaceWith(other)) {
						newObject->standingOn(other); newObject->clampToSurf();
					}
				}
			}
		}
	}

	newObject->rotate(rot);
	newObject->worldScale() = scale;
	
	return newObject;
}

#endif //SPAWNMAN_H
