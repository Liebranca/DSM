#ifndef TRANSFORM_H
#define TRANSFORM_H

//	- --- - --- - --- - --- -

#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "../lymath/vops.h"

#define IDENTITY glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

//	- --- - --- - --- - --- -
namespace threed {
	// 3D transform class
	class DS_TRANSFORM
	{
	public:
		DS_TRANSFORM(const glm::vec3& pos = glm::vec3(0, 0, 0),
			const glm::quat& rot = glm::quat(1, 0, 0, 0),
			const glm::vec3& scale = glm::vec3(1, 1, 1)) {
			wPos = pos; wRot = rot; wScale = scale;
		}

		virtual ~DS_TRANSFORM() {};

		inline glm::mat4 getModel(bool ignoreParent = false) const;
		inline glm::mat4 getNormal(bool ignoreParent = false) const;

//		- --- - --- - --- - --- -

		inline glm::vec3& worldPosition() { return wPos; }
		inline glm::quat& worldOrientation() { return wRot; }
		inline glm::vec3& worldScale() { return wScale; }

		void setParent(DS_TRANSFORM* parent) { this->parent = parent; }

//		- --- - --- - --- - --- -

			// Overcomplicated func to have an object face towards another.
		bool faceTo(glm::vec3 v, float elapsed, float rfac = 10.0f, bool fullRot = false);

		void rotate(glm::quat twat);
		void move(glm::vec3& pos) { wPos += pos; }

		glm::vec3 localizePos(glm::vec3& mvec, glm::vec3 fvec, glm::vec3 lvec);

//		- --- - --- - --- - --- -

	private:
		DS_TRANSFORM* parent = nullptr;

		glm::vec3 wPos;
		glm::quat wRot;
		glm::vec3 wScale;

		glm::vec3 yAxis = glm::vec3(0, 1, 0);
		glm::vec2 dirn = glm::vec2(1, 1);

		float fwd = 0.0f;
		float tick = 0.0f;

	};
}

namespace lyspace {
	using namespace threed;
}

#endif // TRANSFORM_H