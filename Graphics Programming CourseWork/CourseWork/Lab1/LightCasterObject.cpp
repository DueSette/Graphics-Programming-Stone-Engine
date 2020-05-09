#include "LightCasterObject.h"

void LightCasterObject::initialiseLightObject(glm::vec3 pos)
{
	GameObject::initialise(s_kModels + "crate2.obj", s_kTextures + "white.png", s_kShaders + "vanilla.vert", s_kShaders + "vanilla.frag", pos, ColliderType::NONE); //calls baseclass initialiser
	setScale(glm::vec3(0.33, 0.33, 0.33));
}

void LightCasterObject::updateShadersWithLight(Shader* s, std::string lightNum)
{
	s->setVec4("lights[" + lightNum + "].pos", glm::vec4(getPosition(), 1.0));
	s->setVec3("lights[" + lightNum + "].color", color);

	s->setFloat("lights[" + lightNum + "].constantFall", 1.0f);
	s->setFloat("lights[" + lightNum + "].linearFall", linearFalloff);
	s->setFloat("lights[" + lightNum + "].quadraticFall", quadraticfallOff);
}

void LightCasterObject::shiftHue(float delta)
{
	color.r = sinf(delta);
	color.g = -sinf(delta);
	color.b = cosf(delta);
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
