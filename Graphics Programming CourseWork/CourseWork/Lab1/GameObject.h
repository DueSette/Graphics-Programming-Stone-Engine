#pragma once
#include <GL\glew.h>
#include "Utilities.h"
#include "Transform.h"
#include "Model.h"
#include "Shader.h"

/*
	The point of this class is to make every element more manageable
	It handles meshes, textures, shaders, colliders and their components
	Comments are available in the .cpp file
*/

class GameObject
{
public:
	GameObject();

	void initialise(const std::string& meshName, const std::string& textureName, const std::string& vertShader, const std::string& fragShader, glm::vec3 pos, ColliderType ct);
	void initialise(const std::string& meshName, const std::string& textureName, const std::string& vertShader, const std::string& geoShader, const std::string& fragShader, glm::vec3 pos, ColliderType ct);
	
	Shader* exposeShaderProgram();
	void drawProcedure(const Camera& cam);

	void AddTextureMap(const std::string& fileName);

	glm::vec3 getPosition();
	glm::vec3 getRotation();

	void setPosition(glm::vec3 pos);
	void setRotation(glm::vec3 rot);	
	void setScale(glm::vec3 scale);

	void setColliderSize(float x, float y, float z);

	void translate(glm::vec3 translation);
	void rotate(glm::vec3 rotation);

	bool addCollider(ColliderType t);
	Collider* getCollider();
	
	glm::mat4 getModel();

	std::string _name;

protected:
	void Setup();

	Transform* _transform;
	Mesh* _mesh;
	Texture* _baseMap, * _specularMap;
	Shader* _shader;
	Collider* _collider = nullptr;
};
