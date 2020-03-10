#include "Model.h"

/*
==========
The data retrievable from these classes allows us to check for collisions
==========
*/

SphereCollider::SphereCollider()
{
}

SphereCollider::SphereCollider(const glm::vec3& position, float radius)
	:radius(radius)
{
	print("Created Sphere Collider", DebugMessageTier::INFO);
}

glm::vec3 SphereCollider::getSize()
{
	return glm::vec3(radius, radius, radius);
}

float* SphereCollider::getRadius()
{
	return &radius;
}

void SphereCollider::setRadius(float newRadius)
{
	radius = newRadius;
}

BoxCollider::BoxCollider()
	:sizeX(1), sizeY(1), sizeZ(1)
{
}

BoxCollider::BoxCollider(const glm::vec3& position, float x, float y, float z)
	:sizeX(x), sizeY(y), sizeZ(z)
{
	print("Created Box Collider", DebugMessageTier::INFO);
}

glm::vec3 BoxCollider::getSize()
{
	return glm::vec3(sizeX, sizeY, sizeZ);
}

void BoxCollider::setSize(float x, float y, float z)
{
	sizeX = x;
	sizeY = y;
	sizeZ = z;
}

//we set this to default -100 because it's safe to assume we will never manually call it with such parameters
void BoxCollider::scaleSize(float x = -100, float y = -100, float z = -100) //this signature allows us to call the method with only some parameters
{
	if (sizeX != -100)
		sizeX *= x;
	if (sizeY != -100)
		sizeY *= y;
	if (sizeZ != -100)
		sizeZ *= z;
}