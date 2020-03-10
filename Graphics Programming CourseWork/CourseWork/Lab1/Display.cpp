#include "Display.h"

Display::Display()
{
	_screenInfo.height = 720;
	_screenInfo.width = 1080;
}

Display::~Display()
{
	SDL_GL_DeleteContext(_context); // delete context
	SDL_DestroyWindow(_window); // detete window (make sure to delete the context and the window in the opposite order of creation in initDisplay())
	SDL_Quit();
}

void Display::initialise()
{
	SDL_Init(SDL_INIT_EVERYTHING); //initalise everything

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); //Min no of bits used to diplay colour
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);// set up z-buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // set up double buffer   

	_window = SDL_CreateWindow("Andrea Caramia Coursework", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, getInfo().width, getInfo().height, SDL_WINDOW_OPENGL); // create window

	if (_window == nullptr)
	{
		logError("Window Creation Failed");
	}

	_context = SDL_GL_CreateContext(_window);

	if (_context == nullptr)
	{
		logError("SDL_GL context creation Failed");
	}

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		logError("GLEW init Failed");
	}

	glEnable(GL_DEPTH_TEST); //enable z-buffering 
	glEnable(GL_CULL_FACE); //dont draw faces that are not pointing at the camera

	glClearColor(1.0f, 0.4f, 1.0f, 1.0f);
}

void Display::swapBuffer()
{
	SDL_GL_SwapWindow(_window); //for extra crispness
}

void Display::clearDisplay(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color and depth buffer
}

ScreenInfo Display::getInfo()
{
	return _screenInfo;
}

void Display::logError(std::string errorString)
{
	std::cout << errorString << std::endl << "Press any button";
	std::cin.get();
	SDL_Quit();
}
