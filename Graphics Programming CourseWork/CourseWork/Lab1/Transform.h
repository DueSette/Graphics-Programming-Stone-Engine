#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "Camera.h"

//Quick macros for the 4 fundamental directions
#define VECTOR_UP glm::vec3(0, 1, 0)
#define VECTOR_RIGHT glm::vec3(1, 0, 0)
#define VECTOR_FORWARD glm::vec3(0, 0, 1)
#define VECTOR_ZERO glm::vec3(0, 0, 0)

struct Transform
{
public:
	inline Transform(const glm::vec3& pos = glm::vec3(), const glm::vec3& rot = glm::vec3(), const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f))
		:pos(pos), rot(rot), scale(scale)
	{		
	}

	inline glm::mat4 GetModel() const
	{
		glm::mat4 posMat = glm::translate(pos);
		glm::mat4 scaleMat = glm::scale(scale);
		glm::mat4 rotX = glm::rotate(rot.x, glm::vec3(1.0, 0.0, 0.0));
		glm::mat4 rotY = glm::rotate(rot.y, glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 rotZ = glm::rotate(rot.z, glm::vec3(0.0, 0.0, 1.0));
		glm::mat4 rotMat = rotX * rotY * rotZ;
		
		return posMat * rotMat * scaleMat;
	}

	/*inline glm::mat4 GetMVP(const Camera& camera) const
	{
		glm::mat4 VP = camera.GetViewProjection();
		glm::mat4 M = GetModel();

		return VP * M;//camera.GetViewProjection() * GetModel();
	}*/

	inline glm::vec3* GetPos() { return &pos; } //getters
	inline glm::vec3* GetRot() { return &rot; }
	inline glm::vec3* GetScale() { return &scale; }

	inline void SetPos(const glm::vec3& pos) { this->pos = pos; } // setters
	inline void SetRot(const glm::vec3& rot) { this->rot = rot; }
	inline void SetScale(const glm::vec3& scale) { this->scale = scale; }
protected:
private:
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale;
};


