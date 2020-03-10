#pragma once
#include <SDL/SDL.h>
#include <GL\glew.h>
#include <iostream>
#include <string>

struct ScreenInfo
{
	float width;
	float height;
};

class Display
{
public:
	Display();
	~Display();

	void initialise();
	void swapBuffer();
	void clearDisplay(float r, float g, float b, float a);

	ScreenInfo getInfo();
	SDL_Window* _window; //holds pointer to out window

private:

	void logError(std::string errorString);

	SDL_GLContext _context; //global variable to hold the context
	
	ScreenInfo _screenInfo;
};
