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

	void renderMonkeys();

	//Shadowmapping processes
	void setShadowMap();
	void configureDirectionalLightPerspective();
	Shader* _depthShader = nullptr;
	unsigned int depthMapFrameBuffer;
	unsigned int depthMapTexture;
	glm::mat4 directionalLightPerspective;
	glm::vec3 directionalLightPosition = glm::vec3(70.0f, 90.0f, -33.0f);

	//HDR and framebuffer processes
	void setHDR();
	unsigned int hdrFrameBuffer;
	unsigned int hdrTextures[2]; //we have two hdrTextures that we will attach to the hdr framebuffer, one is the normal color, the other is used for bloom
	Shader* _tonemapperShader;
	float exposure = 1.5f;

	//Bloom processes
	void setBloom();
	Shader* _blurShader;
	unsigned int pingpongTextures[2];
	unsigned int pingpongFramebuffers[2];
	int blurPassesAmount = 10;

	//Skybox methods and fields
	GLuint skyboxVAO, skyboxVBO, cubemapTexture;
	Shader* _skyboxShader;
	Texture skybox;
	void setSkybox();
	void renderSkybox();

	//In-scene illumination 
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

	GameObject _roof0, _roof1, _giantMonkey, _explodingMonkey, _environmentMonkey, _dolphin;

	LightCasterObject _pointLight0, _pointLight1, _pointLight2;

	//Sound clips
	unsigned int objectSpawnSound;
	unsigned int backGroundMusic;

	//INPUT
	void inputUpdate();

	float camSpeedX = 0.0f;
	float camSpeedZ = 0.0f;
};
