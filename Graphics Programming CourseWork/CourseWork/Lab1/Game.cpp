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
		vec3(0.0f, 2.0f, -10.0f), //Starting pos
		70.0f, //FOV
		(float)_gameDisplay.getInfo().width / _gameDisplay.getInfo().height, //Aspect ratio
		0.01f, //Near clip
		1000.0f //Far clip
	);

	glEnable(GL_DEPTH_TEST);

	setupStartingScene();
	setShadowMap();
	setHDR();
	setBloom();
	configureDirectionalLightPerspective();
}

void Game::setupStartingScene()
{
	//MAP, SCENE MEMBERS
	_map.initialise(s_kModels + "map.obj", s_kTextures + "concrete.png", s_kShaders + "vertex_blinn_phong.vert", s_kShaders + "fragment_blinn_phong.frag", glm::vec3(0, -1, 0), ColliderType::BOX);
	_map.isKinematic = true;

	_map.setScale(glm::vec3(20, 20, 20));
	_map.setColliderSize(30, 0.6f, 30);
	_map.setPosition(-VECTOR_UP * 3.0f);
	_map.AddSpecularMap(s_kTextures + "concrete.png");

	_box0.initialise(s_kModels + "crate2.obj", s_kTextures + "crate_basemap.png", s_kShaders + "vertex_blinn_phong.vert", s_kShaders + "fragment_blinn_phong.frag", glm::vec3(0, 1, 0), ColliderType::NONE);
	_box0.AddSpecularMap(s_kTextures + "crate_specular.png");

	_box1.initialise(s_kModels + "crate2.obj", s_kTextures + "crate_basemap.png", s_kShaders + "vertex_blinn_phong.vert", s_kShaders + "fragment_blinn_phong.frag", glm::vec3(4, 1, 0), ColliderType::NONE);
	_box1.AddSpecularMap(s_kTextures + "crate_specular.png");

	_explodingMonkey.initialise(s_kModels + "monkey3.obj", s_kTextures + "grid.png", s_kShaders + "vertex_explosionShader.vert", s_kShaders + "geometry_explosionShader.geom", s_kShaders + "fragment_explosionShader.frag", glm::vec3(-4, -4, 0), ColliderType::NONE);
	_phongMonkey.initialise(s_kModels + "monkey3.obj", s_kTextures + "concrete.png", s_kShaders + "vertex_phong.vert", s_kShaders + "fragment_phong.frag", glm::vec3(-4, 1, 0), ColliderType::NONE);
	_environmentMonkey.initialise(s_kModels + "monkey3.obj", s_kTextures + "concrete.png", s_kShaders + "vertex_environment.vert", s_kShaders + "fragment_environment.frag", glm::vec3(2, 2, 5), ColliderType::NONE);
	
	//LIGHTBULBS
	_pointLight0.initialiseLightObject(glm::vec3(-4, 3, -5));
	_pointLight0.setLightProperties(PointLightRange::SMALL, COLOR_BLUE);

	_pointLight1.initialiseLightObject(glm::vec3(10, 10, 10));
	_pointLight1.setLightProperties(PointLightRange::LARGE, COLOR_RED);

	_pointLight2.initialiseLightObject(glm::vec3(4, 3, 5));
	_pointLight2.setLightProperties(PointLightRange::SMALL, COLOR_GREEN);

	//VECTOR POPULATION
	physicsGameObjectList.push_back(&_map);

	gameObjectList.push_back(&_map);
	gameObjectList.push_back(&_box0);
	gameObjectList.push_back(&_box1);

	lightCastersList.push_back(&_pointLight0);
	lightCastersList.push_back(&_pointLight1);
	lightCastersList.push_back(&_pointLight2);

	//ADDITIONAL SHADERS
	_depthShader = new Shader();
	_depthShader->createShaderProgram(s_kShaders + "vertex_depth.vert", s_kShaders + "fragment_depth.frag");

	_skyboxShader = new Shader();
	_skyboxShader->createShaderProgram(s_kShaders + "shaderSkybox.vert", s_kShaders + "shaderSkybox.frag");
	setSkybox();

	_tonemapperShader = new Shader();
	_tonemapperShader->createShaderProgram(s_kShaders + "vertex_tonemapper.vert", s_kShaders + "fragment_tonemapper.frag");

	_blurShader = new Shader();
	_blurShader->createShaderProgram(s_kShaders + "vertex_gaussian_blur.vert", s_kShaders + "fragment_gaussian_blur.frag");

	//MATERIAL SETTINGS
	_map.setMaterial(0.1f, 64);
	_box0.setMaterial(0.05f, 128);
	_box1.setMaterial(0.05f, 128);

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
		logicLoop();
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

		case SDL_KEYDOWN:
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_1:
				exposure = 1;
				break;
			case SDLK_2:
				exposure = 2;
				break;
			case SDLK_3:
				exposure = 3;
				break;
			case SDLK_4:
				exposure = 4;
				break;
			case SDLK_5:
				exposure = 5;
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

bool Game::checkCollisions(glm::vec3 s1, glm::vec3 s2, glm::vec3& pos1, glm::vec3& pos2)
{
	if (abs(pos1.x - pos2.x) < s1.x + s2.x)
		if (abs(pos1.y - pos2.y) < s1.y + s2.y)
			return (abs(pos1.z - pos2.z) < s1.z + s2.z);

	return false;
}

void Game::logicLoop()
{
	_box0.rotate(VECTOR_UP * 0.006f); //TODO MAKE THIS SEPARATE LOGIC LOOP
	_box1.rotate(VECTOR_RIGHT * 0.005f);
	_box0.translate(VECTOR_UP * 0.01f * sin(counter));

	_pointLight0.translate(VECTOR_UP * 0.1f * sin(counter));
	_pointLight1.translate(VECTOR_RIGHT * 0.1f * sin(counter));
	_pointLight2.translate(VECTOR_FORWARD * 0.1f * sin(counter));

}

#pragma region ====== RENDERING RELATED METHODS ========
void Game::renderLoop() //where all the rendering is called from
{
	_gameDisplay.clearDisplay(1.0, 0.2, 0.2, 0); //Screen is cleared to red to make it easy to spot the difference between rendering wrong and not rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//DEPTH PASS LOOP	
	_depthShader->bind(); //we set the shader that will write to the depth texture
	_depthShader->setMat4("lightSpaceMatrix", directionalLightPerspective);

	glViewport(0, 0, 1024, 1024); //resolution of the shadowmap, not the actual screen
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFrameBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	for (GameObject* g : gameObjectList) //Retrieves all shader-related informations and issues draw call
	{
		g->drawShadowMap(_depthShader);
	}

	//RESET BUFFER DATA
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFrameBuffer); //unbind depth framebuffer and bind HDR buffer
	glViewport(0, 0, _gameDisplay.getInfo().width, _gameDisplay.getInfo().height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//HDR PASS LOOP (this is the actual rendering of the scene, but we save the values to the HDR framebuffer for additional processing)

	for (GameObject* g : gameObjectList) //Retrieves all shader-related informations and issues draw call
	{	
		Shader* s = g->exposeShaderProgram();
		if (!g->hasMaterial()) { continue; } //if the object has no material it means it probably isn't lit

		g->updateShaderWithMaterial(s); //puts material data into lit shader

		glm::mat4 modelMatrix = g->getModel();
		s->setVec3("dirLight.position", directionalLightPosition);
		s->setVec3("dirLight.color", glm::vec3(0.5f, 0.5f, 0.5f));

		s->setMat4("ModelMatrix", modelMatrix);
		s->setVec3("CameraPosition", _player.cam.getPosition());
		s->setMat4("lightPerspectiveMatrix", directionalLightPerspective);
		retrieveLightData(s); //puts lights data into lit shader
		
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);

		s->setInt("shadowMap", 2);
		g->drawProcedure(_player.cam);
	}

	for (LightCasterObject* l : lightCastersList)
	{
		Shader* s = l->exposeShaderProgram();
		s->setVec3("inColor", l->getColor());
		l->drawProcedure(_player.cam);
	}

	{
		//EXPLODING SHADER ONLY, this is for the lab assignment task
		Shader* s = _explodingMonkey.exposeShaderProgram();
		s->setFloat("u_Time", counter);
		_explodingMonkey.drawProcedure(_player.cam);

		//PHONG-SHADING ONLY, this is for the lab assignment task
		s = _phongMonkey.exposeShaderProgram();
		s->setMat4("ModelMatrix", _phongMonkey.getModel());
		s->setVec3("LightPosition", directionalLightPosition);
		s->setVec3("LightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		s->setVec3("CameraPosition", _player.cam.getPosition());
		s->setVec3("Ka", glm::vec3(0.1, 0.1, 0.2));
		s->setVec3("Kd", glm::vec3(0.3, 0.3, 0.3));
		s->setVec3("Ks", glm::vec3(1.0, 1.0, 0.6));
		s->setFloat("Shininess", 32.0);
		_phongMonkey.drawProcedure(_player.cam);

		//Skybox procedure
		renderSkybox();

		//Environment mapping procedure
		s = _environmentMonkey.exposeShaderProgram();
		s->setVec3("CameraPosition", _player.cam.getPosition());
		s->setMat4("ViewProjectionMatrix", _player.cam.GetViewProjection());
		s->setMat4("ModelMatrix", _environmentMonkey.getModel());
		s->setInt("skyTexture", 0);
		_environmentMonkey.drawProcedure(_player.cam);
	}

	//BLUR PASS
	
	_blurShader->bind();

	horizontalBlurring = true;
	firstBlurPass = true;

	for (unsigned int i = 0; i < blurPassesAmount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFramebuffers[horizontalBlurring]);
		_blurShader->setInt("horizontal", horizontalBlurring);
		glBindTexture(GL_TEXTURE_2D, firstBlurPass ? hdrTextures[1] : pingpongTextures[!horizontalBlurring]);  // bind texture of other framebuffer (or scene if first iteration)
		
		renderQuadInFrontOfCamera(); //we render the previous scene as a texture on a quad, and apply blurring processing on that texture
		horizontalBlurring = !horizontalBlurring;

		if (firstBlurPass) { firstBlurPass = false; }
	}

	//BLOOM AND TONEMAPPING PASS
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //we switch to the default framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0); //we bind the render texture that was blurred in the previous post-processing step
	glBindTexture(GL_TEXTURE_2D, hdrTextures[0]);
	glActiveTexture(GL_TEXTURE1); //we bind 
	glBindTexture(GL_TEXTURE_2D, pingpongTextures[!horizontalBlurring]);

	_tonemapperShader->bind();
	_tonemapperShader->setInt("hdrBufferTexture", 0); //we feed that same texture as a sampler for the tonemapping shader, which will apply postprocess
	_tonemapperShader->setInt("brightBlurredTexture", 1); //we feed that same texture as a sampler for the tonemapping shader, which will apply postprocess
	_tonemapperShader->setFloat("exposure", exposure);

	renderQuadInFrontOfCamera();//we render a quad while the tonemapper is bound and output the scene
	
	_gameDisplay.swapBuffer();
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

void Game::configureDirectionalLightPerspective()
{
	//we use a directional light and test depth from its perspective (orthogonal projection, because the light is directional)

	float nearClip = 1.0f, farClip = 100.0f;
	//create projection matrix
	glm::mat4 lightProjection = glm::perspective(70.0f, (float)_gameDisplay.getInfo().width / _gameDisplay.getInfo().height, //FoV and aspect ratio
		nearClip, farClip); //clipping planes still exist, objects outside will not produce a shadow map (won't be tested for depth)

	//create light look-at matrix (AKA view matrix)
	glm::mat4 lightView = glm::lookAt(directionalLightPosition, //light pos
		glm::vec3(0, 0, 0), //position the light is looking at
		glm::vec3(0, 1, 0)); //simply up vector

	directionalLightPerspective = lightProjection * lightView; //this resulting matrix is not very different from an MVP one (minus the model)
}

void Game::setShadowMap()
{
	//SHADOWMAPPING - we set a shadowmap buffer: it draws to a texture where only depth is stored

	// ======== generate framebuffer handle
	glGenFramebuffers(1, &depthMapFrameBuffer);

	//========= generate texture to which attach the framebuffer's depth map
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	//basic texture parameterisation (almost always the same process)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//========= attach texture as the target of the depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFrameBuffer); //we need to bind the buffer or it will attach the texture to the game screen buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE); //this and the line below are necessary to let openGL know that we are not using color data, only depth
	glReadBuffer(GL_NONE);
}

void Game::setHDR()
{
	glGenFramebuffers(1, &hdrFrameBuffer); //we create a new framebuffer used for the HDR pass
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFrameBuffer);
	// create floating point textures
	glGenTextures(2, hdrTextures);
	for (int i = 0; i < sizeof(hdrTextures) / sizeof(*hdrTextures); i++) //we repeat this process once for the hdr texture and once for the bloom texture
	{
		std::cout << i << " ITER ITER " << std::endl;
		glBindTexture(GL_TEXTURE_2D, hdrTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1080, 720, 0, GL_RGBA, GL_FLOAT, NULL); //These textures are FLOATING POINT, so they can store high dynamic range
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, hdrTextures[i], 0); //we bind both to the same framebuffer
	}

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments); //we tell the framebuffer to store TWO outputs from the fragment shader, it does not happen automatically

	// create depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1080, 720);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth); 	// Attach depth component to renderbuffer
}

void Game::setBloom()
{
	glGenFramebuffers(2, pingpongFramebuffers); //Create two framebuffers which will switch back and forth
	glGenTextures(2, pingpongTextures); //Unlike HDR framebuffer, this time we have two framebuffers with two textures. Not one FBO with two attachments.

	for (unsigned int i = 0; i < 2; i++) //Bind textures to framebuffers
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFramebuffers[i]); //select buffer

		glBindTexture(GL_TEXTURE_2D, pingpongTextures[i]); //bind and define floating point textures (like the HDR ones they have high-range values)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1080, 720, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongTextures[i], 0);
	}
}

void Game::setSkybox()
{
	std::vector<std::string> faces
	{
		"..\\res\\textures\\skybox\\right.jpg",
		"..\\res\\textures\\skybox\\left.jpg",
		"..\\res\\textures\\skybox\\top.jpg",
		"..\\res\\textures\\skybox\\bottom.jpg",
		"..\\res\\textures\\skybox\\front.jpg",
		"..\\res\\textures\\skybox\\back.jpg"
	};

	cubemapTexture = skybox.loadCubemap(faces); //Load the cubemap using "faces" into cubemapTextures

	float skyboxVertices[] = {
		// positions          
		-150.0f,  150.0f, -150.0f,
		-150.0f, -150.0f, -150.0f,
		150.0f, -150.0f, -150.0f,
		150.0f, -150.0f, -150.0f,
		150.0f,  150.0f, -150.0f,
		-150.0f,  150.0f, -150.0f,

		-150.0f, -150.0f,  150.0f,
		-150.0f, -150.0f, -150.0f,
		-150.0f,  150.0f, -150.0f,
		-150.0f,  150.0f, -150.0f,
		-150.0f,  150.0f,  150.0f,
		-150.0f, -150.0f,  150.0f,

		150.0f, -150.0f, -150.0f,
		150.0f, -150.0f,  150.0f,
		150.0f,  150.0f,  150.0f,
		150.0f,  150.0f,  150.0f,
		150.0f,  150.0f, -150.0f,
		150.0f, -150.0f, -150.0f,

		-150.0f, -150.0f,  150.0f,
		-150.0f,  150.0f,  150.0f,
		 150.0f,  150.0f,  150.0f,
		 150.0f,  150.0f,  150.0f,
		 150.0f, -150.0f,  150.0f,
		-150.0f, -150.0f,  150.0f,

		-150.0f,  150.0f, -150.0f,
		 150.0f,  150.0f, -150.0f,
		 150.0f,  150.0f,  150.0f,
		 150.0f,  150.0f,  150.0f,
		-150.0f,  150.0f,  150.0f,
		-150.0f,  150.0f, -150.0f,

		-150.0f, -150.0f, -150.0f,
		-150.0f, -150.0f,  150.0f,
		 150.0f, -150.0f, -150.0f,
		 150.0f, -150.0f, -150.0f,
		-150.0f, -150.0f,  150.0f,
		 150.0f, -150.0f,  150.0f
	};

	//use openGL functionality to generate & bind data into buffers
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Game::renderSkybox()
{
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	_skyboxShader->bind();
	_skyboxShader->setInt("skybox", 0);
	_skyboxShader->setMat4("viewProjection", _player.cam.GetViewProjection());

	// skybox cube

	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}

void Game::renderQuadInFrontOfCamera() //manually places a quad in front of camera
{
	static unsigned int quadVAO = 0; //making the buffers static prevents the memory leak that comes from binding a new VAO/VBO all the time
	static unsigned int quadVBO;

	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO); //Vertex array object
		glGenBuffers(1, &quadVBO); //Vertex buffer object
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
#pragma endregion