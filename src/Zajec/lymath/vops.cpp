#include "vops.h"
glm::vec3 ZVEC3 = { 0.0f,0.0f,0.0f };

//	- --- - --- - --- - --- -

namespace vops {
	glm::vec3 xzvec(glm::vec3 v) {
		return { v.x, 0, v.z };
	}
}