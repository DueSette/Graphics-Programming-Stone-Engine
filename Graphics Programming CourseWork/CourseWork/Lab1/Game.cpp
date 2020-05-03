#include "Game.h"
#include "Time.h"

Game::Game()
	:_gameState(GameState::PLAY)
{}

Game::~Game()
{}

void Game::run()
{
	init();
	gameLoop();
}

void Game::init()
{
	_gameDisplay.initialise();
	SDL_SetRelativeMouseMode(SDL_TRUE);					  
	counter = 1.0f;

	_player.cam.initialiseCamera
	(
		vec3(0.0f, 2.0f, -10.0f),
		70.0f,
		(float)_gameDisplay.getInfo().width / _gameDisplay.getInfo().height,
		0.01f,
		1000.0f
	);

	setupStartingScene();
}

void Game::setupStartingScene()
{
	//MAP, SCENE MEMBERS
	_map.initialise(s_kModels + "map.obj", s_kTextures + "bricks.jpg", s_kShaders + "vertex_phong.vert", s_kShaders + "fragment_blinn_phong.frag", glm::vec3(0, -1, 0), ColliderType::BOX);
	_map.isKinematic = true;

	_map.setScale(glm::vec3(20, 20, 20));
	_map.setColliderSize(30, 0.6f, 30);
	_map.setPosition(-VECTOR_UP * 3.0f);

	_box0.initialise(s_kModels + "crate2.obj", s_kTextures + "crate_basemap.png", s_kShaders + "vertex_phong.vert", s_kShaders + "fragment_blinn_phong.frag", glm::vec3(0, 1, 0), ColliderType::NONE);
	_box0.AddTextureMap(s_kTextures + "crate_specular.png");

	_box1.initialise(s_kModels + "crate2.obj", s_kTextures + "crate_basemap.png", s_kShaders + "vertex_phong.vert", s_kShaders + "fragment_blinn_phong.frag", glm::vec3(4, 1, 0), ColliderType::NONE);

	_explodingMonkey.initialise(s_kModels + "monkey3.obj", s_kTextures + "grid.png", s_kShaders + "vertex_explosionShader.vert", s_kShaders + "geometry_explosionShader.geom", s_kShaders + "fragment_explosionShader.frag", glm::vec3(-4, -4, 0), ColliderType::NONE);
	
	//LIGHTBULBS
	_pointLight0.initialiseLightObject(glm::vec3(-4, 3, -5));
	_pointLight0.setLightProperties(PointLightRange::SMALL, COLOR_BLUE);

	_pointLight1.initialiseLightObject(glm::vec3(10, 10, 10));
	_pointLight1.setLightProperties(PointLightRange::MEDIUM, VECTOR_ZERO);

	_pointLight2.initialiseLightObject(glm::vec3(4, 3, 5));
	_pointLight2.setLightProperties(PointLightRange::SMALL, COLOR_GREEN);

	//VECTOR POPULATION
	physicsGameObjectList.push_back(&_map);
	gameObjectList.push_back(&_box0);
	gameObjectList.push_back(&_box1);

	lightCastersList.push_back(&_pointLight0);
	lightCastersList.push_back(&_pointLight1);
	lightCastersList.push_back(&_pointLight2);

	//MATERIAL SETTINGS
	_map.setMaterial(0.1f, 32);
	_box0.setMaterial(0.1f, 256);
	_box1.setMaterial(0.4f, 256);

	//SOUND
	objectSpawnSound = audioManager.loadSound("..\\res\\audio\\dolphin.wav");
	backGroundMusic = audioManager.loadSound("..\\res\\audio\\vapor.wav");
	audioManager.setlistener(VECTOR_ZERO, VECTOR_FORWARD);
}

void Game::gameLoop()
{
	while (_gameState != GameState::EXIT)
	{		
		counter += 0.02f;
		++frames;

		deltaTime = (float)updateDeltaTime();

		inputUpdate();
		physicsLoop();
		renderLoop();

		playAudioNoRepeat(backGroundMusic, VECTOR_ZERO);
	}
}

void Game::inputUpdate()
{
	SDL_Event e;
	//reading the keyboard state enables us to take more than one input at a time
	//without it, moving diagonally would be very blocky
	const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			if (keyboard_state[SDL_SCANCODE_W])
			{
				camSpeedZ = 17.0f * deltaTime;
			}

			if (keyboard_state[SDL_SCANCODE_S])
			{
				camSpeedZ = -17.0f * deltaTime;
			}

			if (keyboard_state[SDL_SCANCODE_A])
			{
				camSpeedX = -17.0f * deltaTime;
			}
			if (keyboard_state[SDL_SCANCODE_D])
			{
				camSpeedX = 17.0f * deltaTime;
			}
		}

		else if (e.type == SDL_KEYUP && e.key.repeat == 0)
		{
			if (!keyboard_state[SDL_SCANCODE_W])
			{
				if (camSpeedZ > 0)
					camSpeedZ = 0;
			}

			if (!keyboard_state[SDL_SCANCODE_S])
			{
				if (camSpeedZ < 0)
					camSpeedZ = 0;
			}

			if (!keyboard_state[SDL_SCANCODE_A])
			{
				if (camSpeedX < 0)
					camSpeedX = 0;
			}
			if (!keyboard_state[SDL_SCANCODE_D])
			{
				if (camSpeedX > 0)
					camSpeedX = 0;
			}
		}

		switch (e.type)
		{
			//=======================IN CASE OF QUITTING
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;

			//=======================IN CASE OF MOUSE MOV
		case SDL_MOUSEMOTION:
		{
			_player.cam.pitch(e.motion.yrel * deltaTime * 0.3f); //rotates up and down
			_player.cam.yaw(e.motion.xrel * deltaTime * -0.3f); //rotates left and right
			break;
		}
		break;
		case SDL_KEYUP:
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_SPACE:
				_player.jump(0.35f);
				break;			
			}
			break;
		}

		case SDL_MOUSEWHEEL:
			_player.cam.moveOnZ(e.wheel.y * 4.0f);
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (e.button.button == SDL_BUTTON_LEFT)
			{
				PhysicsGameObject* g = new PhysicsGameObject;

				glm::vec3 gPos = glm::vec3(_player.cam.getPosition().x + _player.cam.getForward().x * 3.5f, 5.5f, _player.cam.getPosition().z + _player.cam.getForward().z * 3.5f);
				glm::vec3 gDir = (glm::vec3)normalize(gPos - _player.cam.getPosition());
				gDir.y = 0;
				g->initialiseRandom(gPos); //instantiating a random-assembled object with the newly calculated position (always in front and above player)
				g->addForce(gDir * 5.0f);

				g->mass = 0.6f;
				physicsGameObjectList.push_back(g);
				audioManager.playSound(objectSpawnSound);
			}
			break;
		}
	}

	//cam speeds are stored separately to add a step 
	//of separation between movement and on-screen results
	_player.cam.moveOnX(camSpeedX);
	_player.cam.moveOnZ(camSpeedZ);
}

void Game::physicsLoop()
{
	_player.updatePlayer();

	// ===== COLLISIONS
	for (int i = 0; i < physicsGameObjectList.size(); i++)
	{
		PhysicsGameObject* g1 = physicsGameObjectList[i];
		Collider* colA = g1->getCollider();

		BoxCollider* boxCollider1 = nullptr;	//these are needed for dynamic casting, one of them will stay null
		SphereCollider* sphereCollider1 = nullptr;

		if (colA == nullptr) //if the base pointer is null skip loop (some objects might not have colliders)
			continue;

		boxCollider1 = dynamic_cast<BoxCollider*> (colA);
		if (!boxCollider1)
			sphereCollider1 = dynamic_cast<SphereCollider*> (colA);

		for (int l = 0; l < physicsGameObjectList.size(); l++)
		{
			PhysicsGameObject* g2 = physicsGameObjectList[l];
			Collider* colB = g2->getCollider();

			BoxCollider* boxCollider2 = nullptr;
			SphereCollider* sphereCollider2 = nullptr;

			if (colB == nullptr || colA == colB)
				continue;

			boxCollider2 = dynamic_cast<BoxCollider*> (colB);
			if (!boxCollider2)
				sphereCollider2 = dynamic_cast<SphereCollider*> (colB);

			if (!boxCollider1 && !boxCollider2)
			{
				if (checkCollisions(sphereCollider1->getSize(), sphereCollider2->getSize(), g1->getPosition(), g2->getPosition()))
					std::cout << colA << " collided with " << colB << " SPHERE VS SPHERE" << std::endl;
			}

			else if (!boxCollider1 && boxCollider2)
			{
				if (checkCollisions(sphereCollider1->getSize(), boxCollider2->getSize(), g1->getPosition(), g2->getPosition()))
					std::cout << colA << " collided with " << colB << " SPHERE VS BOX" << std::endl;
			}

			else if (boxCollider1 && !boxCollider2)
			{
				if (checkCollisions(boxCollider1->getSize(), sphereCollider2->getSize(), g1->getPosition(), g2->getPosition()))
				{
					std::cout << colA << " collided with " << colB << "BOX VS SPHERE" << std::endl;
				}
			}

			else if (boxCollider1 && boxCollider2)
			{
				if (checkCollisions(boxCollider1->getSize(), boxCollider2->getSize(), g1->getPosition(), g2->getPosition()))
				{
					PhysicsGameObject* p = (PhysicsGameObject*)boxCollider1->parent;
					p->onCollision();
				}
			}
		}
	}

	for (int i = 0; i < physicsGameObjectList.size(); i++)
	{
		physicsGameObjectList[i]->updatePhysics();
		if (physicsGameObjectList[i]->getPosition().y < -115)
		{
			delete physicsGameObjectList[i];
			physicsGameObjectList.erase(physicsGameObjectList.begin() + i);
		}
		else if(physicsGameObjectList[i]->_name == s_kModels + "bust1.obj")
		{
			physicsGameObjectList[i]->rotate(glm::vec3(0, 0.9f * deltaTime, 0));
		}
	}
}

void Game::retrieveLightData(Shader* s) //updates all the lit shaders with each light's data
{
	int x = 0;
	for (LightCasterObject* light : lightCastersList)
	{
		light->updateShadersWithLight(s, std::to_string(x));
		x++;
	}
}

void Game::renderLoop()
{
	_gameDisplay.clearDisplay(0, 0, 0, 0);

	_box0.rotate(VECTOR_UP * 0.006f);
	//_box1.rotate(VECTOR_RIGHT * 0.005f);

	//separate loops for normal game objects and physics-enabled gameobjects

	for (GameObject* g : gameObjectList)
	{
		Shader* s = g->exposeShaderProgram();
		if (!g->hasMaterial()) { continue; } //if the object has no material it means it probably isn't lit

		g->updateShaderWithMaterial(s); //puts material data into lit shader

		glm::mat4 modelMatrix = g->getModel();
		s->setMat4("ModelMatrix", modelMatrix);
		s->setVec3("CameraPosition", _camera.getPosition());

		retrieveLightData(s); //puts lights data into lit shader

		g->drawProcedure(_player.cam);
	}

	for (PhysicsGameObject* g : physicsGameObjectList)
	{
		Shader* s = g->exposeShaderProgram();
		if (!g->hasMaterial()) { continue; }

		g->updateShaderWithMaterial(s);

		glm::mat4 modelMatrix = g->getModel();
		s->setMat4("ModelMatrix", modelMatrix);
		s->setVec3("CameraPosition", _camera.getPosition());

		retrieveLightData(s);

		g->drawProcedure(_player.cam);
	}

	for (LightCasterObject* l : lightCastersList)
	{
		l->exposeShaderProgram();
		l->drawProcedure(_player.cam);
	}

	Shader* s = _explodingMonkey.exposeShaderProgram();
	s->setFloat("u_Time", counter);
	_explodingMonkey.drawProcedure(_player.cam);

	glEnableClientState(GL_COLOR_ARRAY);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnd();
	_gameDisplay.swapBuffer();
}

//NEW COLLISION METHOD
bool Game::checkCollisions(glm::vec3 s1, glm::vec3 s2, glm::vec3& pos1, glm::vec3& pos2)
{
	if (abs(pos1.x - pos2.x) < s1.x + s2.x)
		if (abs(pos1.y - pos2.y) < s1.y + s2.y)
			return (abs(pos1.z - pos2.z) < s1.z + s2.z);

	return false;
}
