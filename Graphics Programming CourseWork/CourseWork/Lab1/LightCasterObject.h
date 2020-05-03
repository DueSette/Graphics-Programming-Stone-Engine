#pragma once
#include "GameObject.h"
#include "AudioManager.h"
#include "Utilities.h"

typedef enum PointLightRange{ SMALL, MEDIUM, LARGE };

//===========CLASS SUMMARY
//this class is used for game objects that contain light bulbs
class LightCasterObject : public GameObject
{
public:
	void initialiseLightObject();
	void updateShadersWithLight();

	void setLightProperties(PointLightRange radius, glm::vec3 color);

private:
	glm::vec3 color;
	PointLightRange type;
	float linearFalloff, quadraticfallOff;

};