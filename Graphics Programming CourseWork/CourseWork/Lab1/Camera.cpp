#include "Camera.h"

void Camera::initialiseCamera(const vec3& pos, float fov, float aspect, float nearClip, float farClip)
{
	this->pos = pos;
	this->forward = vec3(0.0f, 0.0f, 1.0f);
	this->up = vec3(0.0f, 1.0f, 0.0f);
	this->projection = perspective(fov, aspect, nearClip, farClip);
}

mat4 Camera::GetViewProjection() const 
{
	return projection * glm::lookAt(pos, pos + forward, up);
}

void Camera::moveOnZ(float val)
{
	glm::vec3 oldPos = pos;
	pos += forward * val;
	pos.y = oldPos.y;
}

void Camera::moveOnX(float val)
{
	glm::vec3 oldPos = pos;
	pos += cross(forward, up) * val;
	pos.y = oldPos.y;
}
