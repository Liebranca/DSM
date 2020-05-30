#include "camera.h"
#include <iostream>

DS_CAMERA* active_camera;
glm::mat4 actcam_viewproj;
glm::vec3 actcam_fwd;
glm::vec4 actcam_pos;

glm::vec4 skyColor;

DS_CAMERA::DS_CAMERA(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar)
{
	perspective = glm::perspective(glm::radians(fov), aspect, zNear, zFar);

	this->pos = pos; this->zNear = zNear; this->zFar = zFar;

	this->hNear = 2 * tan(fov / 2) * zFar;
	this->wNear = this->hNear * aspect;
	this->hFar = 2 * tan(fov / 2) * zFar;
	this->wFar = hFar * aspect;

	fwd = glm::vec3(0, 0, 1);
	up = glm::vec3(0, 1, 0);
	size_t six_points = 6 * sizeof(COLFACE);
	planes.reserve(six_points);

}
