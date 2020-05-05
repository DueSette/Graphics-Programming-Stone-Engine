#pragma once
#include "GameObject.h"
#include "AudioManager.h"
#include "Utilities.h"

enum PointLightRange{ SMALL, MEDIUM, LARGE };

//===========CLASS SUMMARY
//this class is used for game objects that contain light bulbs
class LightCasterObject : public GameObject
{
public:
	void initialiseLightObject(glm::vec3 pos);
	void updateShadersWithLight(Shader* s, std::string lightNum);

	void setLightProperties(PointLightRange radius, glm::vec3 color);
	
	inline glm::vec3 getColor() { return color; }
private:
	glm::vec3 color;
	PointLightRange type;
	float linearFalloff, quadraticfallOff;
};