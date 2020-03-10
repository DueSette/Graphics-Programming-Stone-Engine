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
	counter = 0.0f;

	_map.initialise(s_kModels + "map.obj", s_kTextures + "water.jpg", s_kShaders + "vertex_regular.shader", s_kShaders + "fragment_VCR.shader", glm::vec3(0, -1, 0), ColliderType::BOX);
	_map.isKinematic = true;
	_map._name = "Map";
	_map.setScale(glm::vec3(20, 20, 20));
	_map.setColliderSize(30, 0.6f, 30);
	_map.setPosition(-VECTOR_UP * 5.0f);

	_dol0.initialise(s_kModels + "dolf.obj", s_kTextures + "pearly.png", s_kShaders + "vertex_scrollTexture.shader", s_kShaders + "fragment_VCR.shader", glm::vec3(0, 0, 15), ColliderType::NONE);
	_dol0.setRotation(glm::vec3(0, 0, 0));

	_dol1.initialise(s_kModels + "dolf.obj", s_kTextures + "hypnotic.png", s_kShaders + "vertex_scrollTexture.shader", s_kShaders + "fragment_VCR.shader", glm::vec3(0, 0, 15), ColliderType::NONE);
	_dol1.setRotation(glm::vec3(0, 90, 0));

	_dol2.initialise(s_kModels + "dolf.obj", s_kTextures + "grid.png", s_kShaders + "vertex_scrollTexture.shader", s_kShaders + "fragment_VCR.shader", glm::vec3(0, 0, 15), ColliderType::NONE);
	_dol2.setRotation(glm::vec3(0, -90, 0));

	dolphins.push_back(&_dol0);
	dolphins.push_back(&_dol1);
	dolphins.push_back(&_dol2);
	physicsGameObjectList.push_back(&_map);

	//SOUND
	objectSpawnSound = audioManager.loadSound("..\\res\\audio\\dolphin.wav");
	backGroundMusic = audioManager.loadSound("..\\res\\audio\\vapor.wav");
	audioManager.setlistener(VECTOR_ZERO, VECTOR_FORWARD);
	
	_player.cam.initialiseCamera
	(
		vec3(0.0f, 2.0f, -10.0f),
		70.0f,
		(float)_gameDisplay.getInfo().width / _gameDisplay.getInfo().height,
		0.01f,
		1000.0f
	);
}

void Game::gameLoop()
{
	while (_gameState != GameState::EXIT)
	{		
		counter += 0.01f;
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

void Game::renderLoop()
{
	_gameDisplay.clearDisplay(cos(counter) / 2 + 0.55f, -cos(counter) / 4 + 0.25f, 0.4f, 1);

	//render dolphins separately (in a non-demo project this would be avoided at all costs)
	for (int j = 0; j < dolphins.size(); j++)
	{
		dolphins[j]->translate(glm::vec3(1 * cos(counter), 1 * sin(counter), 0) * deltaTime);
		dolphins[j]->rotate(VECTOR_RIGHT * deltaTime);
		dolphins[j]->drawProcedure(_player.cam); //that's how we render every object in the scene
	}

	//separate loops for normal game objects and physics-enabled gameobjects
	for (int i = 0; i < gameObjectList.size(); i++)
		gameObjectList[i]->drawProcedure(_player.cam);

	for (int i = 0; i < physicsGameObjectList.size(); i++)
		physicsGameObjectList[i]->drawProcedure(_player.cam);

	glEnableClientState(GL_COLOR_ARRAY);
	glEnd();
	_gameDisplay.swapBuffer();
}

//LEGACY
bool Game::checkCollisions(glm::vec3 pos1, float rad1, glm::vec3 pos2, float rad2)
{
	float distance = (pos2.x - pos1.x) * (pos2.x - pos1.x) + (pos2.y - pos1.y) * (pos2.y - pos1.y) + (pos2.z - pos1.z) * (pos2.z - pos1.z);
	distance = SDL_sqrt(distance);

	return distance > rad1 + rad2 ? false : true;
}

//NEW COLLISION METHOD
bool Game::checkCollisions(glm::vec3 s1, glm::vec3 s2, glm::vec3& pos1, glm::vec3& pos2)
{
	if (abs(pos1.x - pos2.x) < s1.x + s2.x)
		if (abs(pos1.y - pos2.y) < s1.y + s2.y)
			return (abs(pos1.z - pos2.z) < s1.z + s2.z);

	return false;
}
