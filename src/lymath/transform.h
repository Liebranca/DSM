#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/quaternion.hpp"
#include <glm/gtc/matrix_inverse.hpp>
#include "utils.h"
#include "../backbone/reyn.h"

#define IDENTITY glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

class DS_TRANSFORM
{
public:
	DS_TRANSFORM(const glm::vec3& pos = glm::vec3(0, 0, 0),
		const glm::quat& rot = glm::quat(1, 0, 0, 0),
		const glm::vec3& scale = glm::vec3(1, 1, 1)) {
		wPos = pos; wRot = rot; wScale = scale;
	}

	virtual ~DS_TRANSFORM() {};

	inline glm::mat4 getModel(bool ignoreParent = false) const
	{
		glm::mat4 parentMatrix;
		glm::mat4 wPosMatrix = glm::translate(wPos);
		glm::mat4 wRotMatrix = glm::mat4_cast(glm::normalize(wRot));
		glm::mat4 wScaleMatrix = glm::scale(wScale);

		if (!parent || ignoreParent) { parentMatrix = IDENTITY; }
		else { parentMatrix = parent->getModel(); }

		return parentMatrix * (wPosMatrix * wRotMatrix * wScaleMatrix);
	}
	inline glm::mat4 getNormal(bool ignoreParent = false) const {
		return glm::inverseTranspose(getModel(ignoreParent));
	}

	inline glm::vec3& worldPosition() { return wPos; }
	inline glm::quat& worldOrientation() { return wRot; }
	inline glm::vec3& worldScale() { return wScale; }

	void setParent(DS_TRANSFORM* parent) { this->parent = parent; }

	bool faceTo(glm::vec3 v, float rfac = 10.0f, bool fullRot = false) {
		glm::vec3 vecTo;
		if (!fullRot) { vecTo = -(lymath::xzvec(v) - lymath::xzvec(this->worldPosition())); }
		else { vecTo = -(v - this->worldPosition()); }
		glm::quat twat = glm::quatLookAt(vecTo, yAxis);
		glm::quat& rot = this->worldOrientation();

		if (twat != rot) {
			if (!this->ic) {
				this->ic = new DS_TICKER(10.0f);
			} this->ic->update(reyn->worldClock->getCurr());

			float p = this->ic->getp() / rfac;
			rot = glm::slerp(rot, twat, p); this->rotate({1,0,0,0});
			if (p * rfac * 10.0f > 0.8f) { rot = twat; }
			return p * rfac * 10.0f > 0.3f;
		}
		else if (this->ic) { delete this->ic; this->ic = nullptr; }
		return true;
	}

	void rotate(glm::quat twat) {

		this->worldOrientation() = glm::normalize(glm::cross(wRot, twat));

		fwd = (wRot.y * wRot.w) * 2;
		if (fabs(wRot.w) < 0.5) { dirn.y = -1; }
		else { dirn.y = 1; }

		if (fabs(wRot.y) > 0.75) { dirn.x = -1; }
		else { dirn.x = 1; }

	}

	glm::vec3 localizePos(glm::vec3& mvec, glm::vec3 fvec, glm::vec3 lvec) {
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

	void move(glm::vec3& pos) { wPos += pos; }


private:
	DS_TRANSFORM* parent = nullptr;
	DS_TICKER* ic = nullptr;

	glm::vec3 wPos;
	glm::quat wRot;
	glm::vec3 wScale;

	glm::vec3 yAxis = glm::vec3(0, 1, 0);
	glm::vec2 dirn = glm::vec2(1, 1);

	float fwd = 0.0f;

};

#endif // TRANSFORM_H