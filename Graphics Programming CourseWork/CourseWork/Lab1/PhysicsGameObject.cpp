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

void PhysicsGameObject::initialise(const std::string& meshName, const std::string& textureName, const std::string& vertShader, const std::string& geoShader, const std::string& fragShader, glm::vec3 pos, ColliderType ct)
{
	GameObject::initialise(meshName, textureName, vertShader, geoShader, fragShader, pos, ct); //calls baseclass initialiser
	getCollider()->parent = this;
}

void PhysicsGameObject::initialiseRandom(glm::vec3 pos)
{
	std::string meshName, textureName, specularName, vertShader, fragShader;
	//We select a random entry from each of these arrays, setting them like this makes the process scalable
	std::string meshNames[] = { "crate2.obj" };
	std::string textureNames[] = { "gold.jpg", "bricks.jpg", "pearly.png", "water.jpg" };
	std::string specularNames[] = { "concrete.png", "hypnotic.png", "wood.png" };
	std::string vertexNames[] = { "blinn_phong.vert" };
	std::string fragmentNames[] = { "blinn_phong.frag" };

	//this is deterministic random generation, not recommended but good enough in this case
	int r = rand() % (sizeof(meshNames) / sizeof(*meshNames));
	meshName = s_kModels + meshNames[r];
	
	r = rand() % (sizeof(textureNames) / sizeof(*textureNames));
	textureName = s_kTextures + textureNames[r];

	r = rand() % (sizeof(specularNames) / sizeof(*specularNames));
	specularName = s_kTextures + specularNames[r];

	r = rand() % (sizeof(vertexNames) / sizeof(*vertexNames));
	vertShader = s_kShaders + vertexNames[r];

	r = rand() % (sizeof(fragmentNames) / sizeof(*fragmentNames));
	fragShader = s_kShaders + fragmentNames[r];

	initialise(meshName, textureName, vertShader, fragShader, pos, ColliderType::BOX);
	AddSpecularMap(specularName);
	_name = meshName;

	setMaterial(0.05f, 128);
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
