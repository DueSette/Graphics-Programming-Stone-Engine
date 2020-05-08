#pragma once
#include <SDL\SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <ostream>
#include <iostream>
#include <string>
#include <chrono>
#include <sstream>
#include <vector>
#include <algorithm>
#include "Model.h"
#include "Display.h"
#include "Shader.h"
#include "Transform.h"
#include "PhysicsGameObject.h"
#include "LightCasterObject.h"
#include "Audio.h"
#include "Player.h"
#include "AudioManager.h"

enum class GameState { PLAY, EXIT, PAUSE };

static float deltaTime = 0.000f;
static float counter;
static int frames = 0;

class Game
{
public:
	Game();
	~Game();
	void run();

private:
	//Fundamental methods
	void init();
	void setupStartingScene();
	void gameLoop();
	void logicLoop();
	void physicsLoop();
	void renderLoop();
	void renderQuadInFrontOfCamera(); //this draws a quad in front of the camera, it is used to process framebuffer textures

	//Shadowmapping processes
	void setShadowMap();
	void ConfigureLightPerspective();

	Shader* _depthShader = nullptr;
	unsigned int depthMapFrameBuffer;
	unsigned int depthMapTexture;
	glm::mat4 directionalLightPerspective;
	glm::vec3 directionalLightPosition = glm::vec3(-10.0f, 14.0f, -1.0f);

	//shader methods
	void retrieveLightData(Shader* s);

	//collider-agnostic collision algorithm
	static bool checkCollisions(glm::vec3 s1, glm::vec3 s2, glm::vec3& pos1, glm::vec3& pos2);

	Player _player;
	Display _gameDisplay;
	GameState _gameState;

	std::vector<GameObject*> gameObjectList;
	std::vector<PhysicsGameObject*> physicsGameObjectList;
	std::vector<LightCasterObject*> lightCastersList;

	PhysicsGameObject _map;

	GameObject _box0, _box1, _explodingMonkey, _phongMonkey, _environmentMonkey;

	LightCasterObject _pointLight0, _pointLight1, _pointLight2;

	//HDR and framebuffer methods
	unsigned int hdrFrameBuffer;
	unsigned int hdrTexture;
	Shader* _tonemapperShader;
	float exposure = 1.5f;
	void setHDR();

	//Skybox methods and fields
	GLuint skyboxVAO, skyboxVBO, cubemapTexture;
	Shader* _skyboxShader;
	Texture skybox;
	void StartupSkybox();
	void DrawSkybox();

	//Sound clips
	unsigned int objectSpawnSound;
	unsigned int backGroundMusic;

	//INPUT
	void inputUpdate();

	float camSpeedX = 0.0f;
	float camSpeedZ = 0.0f;
};
