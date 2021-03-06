#include "GameObject.h"

GameObject::GameObject()
	:_mesh(new Mesh()), _baseMap(nullptr), _specularMap(nullptr), _shader(new Shader()), _transform(new Transform()), _name("")
{}

//General startup function that gets all the other classes and files in one place, one overload lets you select shaders and the other uses "regular" ones
void GameObject::initialise(const std::string& meshName, const std::string& textureName, const std::string& vertShader, const std::string& fragShader, glm::vec3 pos, ColliderType collType = ColliderType::NONE)
{
	_mesh->loadModel(meshName);
	_baseMap = new Texture(textureName);

	_shader->createShaderProgram(vertShader, fragShader);
	_transform->SetPos(pos);
	
	switch (collType) //creates a collider based on the specified parameter
	{
	case ColliderType::NONE:
		_collider = nullptr;
		break;

	case ColliderType::SPHERE:
	{
		glm::vec3 s = *_transform->GetScale();
		float f = max(s.x, s.y);
		f = max(f, s.z);
		_collider = new SphereCollider(*_transform->GetPos(), f);
	}
		break;

	case ColliderType::BOX:
	{
		glm::vec3 s = *_transform->GetScale();
		_collider = new BoxCollider(*_transform->GetPos(), s.x, s.y, s.z);
		break;
	}

	default:
		print("Unknown collider type is being added", DebugMessageTier::ERROR);
		break;
	}
	this->Setup();
}

void GameObject::initialise(const std::string& meshName, const std::string& textureName, const std::string& vertShader, const std::string& geoShader, const std::string& fragShader, glm::vec3 pos, ColliderType collType)
{
	_mesh->loadModel(meshName);
	_baseMap = new Texture(textureName);
	_shader->createShaderProgram(vertShader, geoShader, fragShader);
	_transform->SetPos(pos);

	switch (collType) //creates a collider based on the specified parameter
	{
	case ColliderType::NONE:
		_collider = nullptr;
		break;

	case ColliderType::SPHERE:
	{
		glm::vec3 s = *_transform->GetScale();
		float f = max(s.x, s.y);
		f = max(f, s.z);
		_collider = new SphereCollider(*_transform->GetPos(), f);
	}
	break;

	case ColliderType::BOX:
	{
		glm::vec3 s = *_transform->GetScale();
		_collider = new BoxCollider(*_transform->GetPos(), s.x, s.y, s.z);
		break;
	}

	default:
		print("Unknown collider type is being added", DebugMessageTier::ERROR);
		break;
	}
	this->Setup();
}

//post-initialize, useful for specific methods that need to make sure everything is instanced
void GameObject::Setup()
{
	_transform->SetScale(glm::vec3(1, 1, 1));
}

Shader* GameObject::exposeShaderProgram()
{
	_shader->bind();
	return _shader;
}

void GameObject::drawProcedure(const Camera& cam) //all the graphics-related operations in one place, camera has to be passed in from the main loop of course
{
	_shader->update(*_transform, cam);

	_baseMap->Bind(0);

	if (_specularMap != nullptr) { _specularMap->Bind(1); } //specular map is optional

	_mesh->draw();	
}

void GameObject::drawShadowMap(Shader* s) //updates depth shader for shadow processing
{
	s->setMat4("model", getModel());
	_mesh->draw();
}

void GameObject::AddSpecularMap(const std::string& fileName) //used to add an additional texture on top of the basemap (normal, specular, emission)
{
	_specularMap = new Texture(fileName);
}

glm::vec3 GameObject::getPosition()
{
	return *_transform->GetPos();
}

glm::vec3 GameObject::getRotation()
{
	return *_transform->GetRot();
}

void GameObject::setPosition(glm::vec3 pos)
{
	_transform->SetPos(pos);
}

void GameObject::setRotation(glm::vec3 rot)
{
	_transform->SetRot(rot);
}

void GameObject::setScale(glm::vec3 scale)
{
	_transform->SetScale(scale);
}

void GameObject::setMaterial(float ambient, float shininess)
{
	mat = new Material();
	mat->ambient = ambient;
	mat->shininess = shininess;
}

void GameObject::updateShaderWithMaterial(Shader* s)
{
	s->setInt("mat.diffuse", 0);
	s->setInt("mat.specular", 1);
	s->setFloat("mat.shininess", mat->shininess);
	s->setFloat("mat.ambient", mat->ambient);
}

bool GameObject::hasMaterial()
{
	bool result = mat != nullptr;
	if (!result) { print("Game object has no material!!", DebugMessageTier::WARNING); }
	return result;
}

void GameObject::setColliderSize(float x, float y, float z)
{
	if (_collider == nullptr) { return; } //usual guard clause

	BoxCollider* boxCollider1 = nullptr;	//these are needed for dynamic casting, one of them will stay null
	SphereCollider* sphereCollider1 = nullptr;

	boxCollider1 = dynamic_cast<BoxCollider*> (_collider);
	if (!boxCollider1)
	{
		sphereCollider1 = dynamic_cast<SphereCollider*> (_collider);
		sphereCollider1->setRadius((x + y + z) / 3);
	}

	boxCollider1->setSize(x, y, z);
}

//moves the object's transform
void GameObject::translate(glm::vec3 translation)
{
	_transform->SetPos(*_transform->GetPos() + translation);
}

void GameObject::rotate(glm::vec3 rotation)
{
	_transform->SetRot(*_transform->GetRot() + rotation);
}

//currently not in use but will be useful at an hypothetical later stage
bool GameObject::addCollider(ColliderType t)
{
	if (_collider != nullptr) { return false; } //if we already have a collider, don't add another one (might add collider swap later)

	if (t == ColliderType::BOX)
		_collider = &BoxCollider(*_transform->GetPos(), 1, 1, 1);
	else if (t == ColliderType::SPHERE)
		_collider = &SphereCollider(*_transform->GetPos(), 1);

	return true;
}

Collider* GameObject::getCollider()
{
	return _collider;
}

glm::mat4 GameObject::getModel()
{
	return _transform->GetModel();
}
