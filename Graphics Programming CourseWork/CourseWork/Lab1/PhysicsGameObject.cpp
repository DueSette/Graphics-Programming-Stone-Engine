#include "PhysicsGameObject.h"

PhysicsGameObject::PhysicsGameObject()
{
	if (!bounceSound)
		bounceSound = audioManager.loadSound("..\\res\\audio\\bounce.wav");
}

void PhysicsGameObject::initialise(const std::string& meshName, const std::string& textureName, const std::string& vertShader, const std::string& fragShader, glm::vec3 pos, ColliderType ct)
{
	GameObject::initialise(meshName, textureName, vertShader, fragShader, pos, ct); //calls baseclass initialiser
	getCollider()->parent = this;
}

void PhysicsGameObject::initialiseRandom(glm::vec3 pos)
{
	std::string meshName, textureName, vertShader, fragShader;
	//We select a random entry from each of these arrays, setting them like this makes the process scalable
	std::string meshNames[] = { "bust1.obj", "dolf.obj" };
	std::string textureNames[] = { "grid.png", "checker.png", "bricks.jpg", "pearly.png", "water.jpg" };
	std::string vertexNames[] = { "vertex_regular.shader", "vertex_scrollTexture.shader"};
	std::string fragmentNames[] = { "fragment_vanilla.shader", "fragment_VCR.shader" };

	//this is deterministic random generation, not recommended but good enough in this case
	int r = rand() % (sizeof(meshNames) / sizeof(*meshNames));
	meshName = s_kModels + meshNames[r];

	r = rand() % (sizeof(textureNames) / sizeof(*textureNames));
	textureName = s_kTextures + textureNames[r];

	r = rand() % (sizeof(vertexNames) / sizeof(*vertexNames));
	vertShader = s_kShaders + vertexNames[r];

	r = rand() % (sizeof(fragmentNames) / sizeof(*fragmentNames));
	fragShader = s_kShaders + fragmentNames[r];

	initialise(meshName, textureName, vertShader, fragShader, pos, ColliderType::BOX);
	_name = meshName;

	if (_name == s_kModels + "bust1.obj")
		setScale(glm::vec3(0.01f, 0.01f, 0.01f)); //this is hardcoded in because the internet downloaded model was set to an irregular size
	else
	{
		rotate(glm::vec3(100, 0, 170));
		setScale(glm::vec3(0.06f, 0.06f, 0.06f));
	}
	getCollider()->parent = this;
}

void PhysicsGameObject::onCollision()
{
	if (velocity.y < 0.000f) //simple way to prevent collisions to happen while the object just bounced
	{
		velocity.y = -velocity.y * bounciness;

		if (velocity.y < 0.115000f)
		{
			isKinematic = true;
			velocity.y = 0.0f;
		}
		else
			audioManager.playSound(bounceSound);
	}
}

void PhysicsGameObject::addForce(glm::vec3 force)
{
	velocity += force;
}

void PhysicsGameObject::updatePhysics()
{
	if (isKinematic) { return; }

	float subjectiveGravity;

	if (velocity.y > 0.000000f) //if going upwards have a weaker gravity for better looking simulation
		subjectiveGravity = -4.1500000f * mass;
	else
		subjectiveGravity = -4.6500000f * mass;	//strengthen the gravity while falling

	velocity.y += subjectiveGravity * (fixedTime * fixedTime);
	velocity *= 1.0f - drag * fixedTime;

	setPosition(getPosition() + velocity);
}
