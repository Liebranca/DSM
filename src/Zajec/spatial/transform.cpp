#include "transform.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include <glm/gtc/matrix_inverse.hpp>

//	- --- - --- - --- - --- -
namespace threed {
	glm::mat4 DS_TRANSFORM::getModel(bool ignoreParent) const
	{
		glm::mat4 parentMatrix;
		glm::mat4 wPosMatrix = glm::translate(wPos);
		glm::mat4 wRotMatrix = glm::mat4_cast(glm::normalize(wRot));
		glm::mat4 wScaleMatrix = glm::scale(wScale);

		if (!parent || ignoreParent) { parentMatrix = IDENTITY; }
		else { parentMatrix = parent->getModel(); }

		return parentMatrix * (wPosMatrix * wRotMatrix * wScaleMatrix);
	}

//	- --- - --- - --- - --- -

	glm::mat4 DS_TRANSFORM::getNormal(bool ignoreParent) const {
		return glm::inverseTranspose(getModel(ignoreParent));
	}

//	- --- - --- - --- - --- -

	bool DS_TRANSFORM::faceTo(glm::vec3 v, float elapsed, float rfac, bool fullRot) {

		glm::vec3 vecTo;
		if (!fullRot) { vecTo = -(lymath::xzvec(v) - lymath::xzvec(this->worldPosition())); }
		else { vecTo = -(v - this->worldPosition()); }
		glm::quat twat = glm::quatLookAt(vecTo, yAxis);
		glm::quat& rot = this->worldOrientation();

		if (twat != rot) {

			this->tick += elapsed;

			float p = this->tick / rfac;
			rot = glm::slerp(rot, twat, p); this->rotate({ 1,0,0,0 });
			if (p * rfac * 10.0f > 0.8f) { rot = twat; }
			return p * rfac * 10.0f > 0.3f;
		}
		this->tick = 0.0f; return true;
	}

//	- --- - --- - --- - --- -

	void DS_TRANSFORM::rotate(glm::quat twat) {

		this->worldOrientation() = glm::normalize(glm::cross(wRot, twat));

		fwd = (wRot.y * wRot.w) * 2;
		if (fabs(wRot.w) < 0.5) { dirn.y = -1; }
		else { dirn.y = 1; }

		if (fabs(wRot.y) > 0.75) { dirn.x = -1; }
		else { dirn.x = 1; }

	}

//	- --- - --- - --- - --- -

	glm::vec3 DS_TRANSFORM::localizePos(glm::vec3& mvec, glm::vec3 fvec, glm::vec3 lvec) {
		glm::vec3 displace = { 0,0,0 };
		if (mvec[2]) {
			displace += fvec * mvec[2];
		}
		if (mvec[0]) {
			displace += lvec * mvec[0];
		}

		displace[1] = mvec[1];
		return displace;
	}
}