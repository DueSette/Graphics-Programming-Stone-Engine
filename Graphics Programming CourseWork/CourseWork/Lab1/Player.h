#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "Time.h"
#include "Camera.h"
#include "Audio.h"
#include "Transform.h"
#include "AudioManager.h"

#define GRAVITY glm::vec3(0.0f, -9.81f, 0.0f)

class Player
{
public:
	Player();
	Camera cam;

	void jump(float pwr);
	void updatePlayer();

private:
	void velocityUpdate();
	void init();

	void correctPosition();

	glm::vec3 velocity;
	bool grounded = true;
	float fixedTime = 0.2f; // this is basically unity's fixed timestep, it cannot be replaced by deltaTime as it needs to be a lot more consistent

	unsigned int landSound;
	unsigned int jumpSound;
};