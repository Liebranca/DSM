#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <vector>
#include <math.h>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "../lymath/Gaoler.h"

class DS_CAMERA
{
	public:
		DS_CAMERA(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar);
		inline glm::mat4 getViewProjection() const
		{
			return perspective * glm::lookAt(pos, pos+fwd, yAxis);
		};

		glm::mat4 perspective;
		
		glm::vec3 getEye() { return glm::normalize(pos + fwd); }

		void getFrustum() {
			glm::vec3 hAxis = glm::normalize(glm::cross(fwd, up));

			glm::vec3 fc = pos + fwd * zFar;
			glm::vec3 ftl = fc + (up * (hFar / 2)) - (hAxis * (wFar / 2));
			glm::vec3 ftr = fc + (up * (hFar / 2)) + (hAxis * (wFar / 2));
			glm::vec3 fbl = fc - (up * (hFar / 2)) - (hAxis * (wFar / 2));
			glm::vec3 fbr = fc - (up * (hFar / 2)) + (hAxis * (wFar / 2));

			glm::vec3 nc = pos + fwd * zNear;
			glm::vec3 ntl = nc + (up * (hNear / 2)) - (hAxis * (wNear / 2));
			glm::vec3 ntr = nc + (up * (hNear / 2)) + (hAxis * (wNear / 2));
			glm::vec3 nbl = nc - (up * (hNear / 2)) - (hAxis * (wNear / 2));
			glm::vec3 nbr = nc - (up * (hNear / 2)) + (hAxis * (wNear / 2));

			planes = { COLFACE(ntr,ntl,ftl), COLFACE(nbl,nbr,fbr),
					   COLFACE(ntl,nbl,fbl), COLFACE(nbr,ntr,fbr),
					   COLFACE(ntl,ntr,nbr), COLFACE(ftr,ftl,fbl)};

		}

		bool rectInFrustum(std::vector<glm::vec3> bounds) {
			for (unsigned int i = 0; i < bounds.size(); i++) {
				if (pointInFrustum(bounds[i])) { return true; }
			}
			return false;

		}

		bool pointInFrustum(glm::vec3& point) {
			for (int i = 0; i < 6; i++) {
				if ( (dot(planes[i].normal, point)+planes[i].d) < 0) { return false; }
			}
			return true;

		}

		void move(glm::vec3& mvec, bool local = false) {
			glm::vec3 displace = { 0,0,0 };
			if (local) {
				float xfac = fabs(up.x); if (fwd.x < 0) { xfac *= -1; }
				float zfac = fabs(up.z); if (fwd.z < 0) { zfac *= -1; }

				glm::vec3 noup = { fwd.x + xfac, 0, fwd.z + zfac };
				if (mvec[2]) { displace += noup * mvec[2]; }
				if (mvec[0]) { displace -= glm::cross(noup, yAxis) * mvec[0]; }
				displace.y = mvec.y;
			}
			else { displace = mvec; }
			pos += displace;

		}

		void rotate(glm::vec3& twat) {
			pitch += twat.y; yaw += twat.x;
			if (pitch < -1.49f) { pitch = -1.49f; }
			else if (pitch > 1.49f) { pitch = 1.49f; }

			glm::vec3 hAxis = glm::normalize(glm::cross(yAxis, fwd));

			fwd.x = sin(yaw) * cos(pitch);
			fwd.y = sin(pitch);
			fwd.z = cos(yaw) * cos(pitch);
			fwd = glm::normalize(fwd);

			up = glm::normalize(glm::cross(fwd, hAxis));

		}

		glm::vec3& getFwd() { return fwd; }
		glm::vec3& getPos() { return pos; }
		glm::vec3 getFwdCast(float dist = 3.5f) { return pos + (fwd * dist); }

		void snapToLight(glm::vec3 lightPosition) {
			this->altpos = this->pos;
			this->pos = lightPosition;
		}

		void restorePos() { this->pos = this->altpos; }

	private:
		glm::vec3 pos;
		glm::vec3 fwd;
		glm::vec3 up;

		glm::vec3 altpos;

		std::vector<COLFACE> planes;

		float pitch = 0, yaw = 0, zNear, zFar, hFar, wFar, hNear, wNear;
		glm::vec3 yAxis = glm::vec3(0, 1, 0);

};

extern DS_CAMERA* active_camera;
extern glm::mat4 actcam_viewproj;
extern glm::vec3 actcam_fwd;
extern glm::vec4 actcam_pos;

extern glm::vec4 skyColor;

#endif // CAMERA_H
