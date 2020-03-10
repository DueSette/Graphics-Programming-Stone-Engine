#pragma once
#include "GameObject.h"
#include "AudioManager.h"
#include "Utilities.h"

//===========CLASS SUMMARY
//this class grants gameObjects a small dash of physics (gravity and collision handling)
//It goes without saying that this is not an attempt of actual, hifi rigidbody physics

class PhysicsGameObject : public GameObject
{
public:
	PhysicsGameObject();
	void initialise(const std::string& meshName, const std::string& textureName, const std::string& vertShader, const std::string& fragShader, glm::vec3 pos, ColliderType ct);
	void initialiseRandom(glm::vec3 pos); //selects pseudorandom parameters and then calls initialise
	
	void onCollision();
	void addForce(glm::vec3 force);
	void updatePhysics();

	float mass = 1;
	float bounciness = 1.00000f;
	float drag = 0.45f;

	bool isKinematic = false;

private:
	glm::vec3 velocity;
	float fixedTime = 0.2f; // this is basically unity's fixed timestep, it cannot be replaced by deltaTime as it needs to be a lot more consistent

	unsigned int bounceSound;
};
