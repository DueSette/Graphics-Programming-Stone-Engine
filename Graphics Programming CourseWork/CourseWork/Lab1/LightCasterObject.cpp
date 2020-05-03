#include "LightCasterObject.h"

void LightCasterObject::initialiseLightObject()
{
	//TODO load with lamp model, simple vertex and fragment shaders and purely white texture. Maybe add bloom later
}

void LightCasterObject::updateShadersWithLight()
{
}

void LightCasterObject::setLightProperties(PointLightRange radius, glm::vec3 color)
{
	this->color = color;

	switch (radius) //for the purpose of this project there is no need to make this any more powerful than this
	{
	case PointLightRange::SMALL: //simulates distance of 20 units
		linearFalloff = 0.22f;
		quadraticfallOff = 0.20f;
			
	case PointLightRange::MEDIUM: //simulates distance of 50 units
		linearFalloff = 0.09f;
		quadraticfallOff = 0.32f;
	
	case PointLightRange::LARGE: //simulates distance of 100 units
		linearFalloff = 0.045f;
		quadraticfallOff = 0.0075f;

	default:
		break;
	}
}
