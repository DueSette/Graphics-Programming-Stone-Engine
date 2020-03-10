#pragma once
#include <SDL/SDL.h>
#include <chrono>
#include <iostream>
#include <sstream>
#include "Utilities.h"

//Time variables
static std::chrono::time_point< std::chrono::high_resolution_clock > s_lastTime;
static std::chrono::time_point< std::chrono::high_resolution_clock > s_currentTime;
static std::chrono::duration<double> s_doubleDeltaTime;

static float timeScale = 1.0f;

inline void printDeltaTime() //only here for debugging purposes
{
	std::ostringstream stringStream;
	stringStream << s_doubleDeltaTime.count();
	std::string str = stringStream.str();
	print(str, DebugMessageTier::WARNING);
}

inline double updateDeltaTime()
{
	s_currentTime = std::chrono::high_resolution_clock::now();
	s_doubleDeltaTime = (s_currentTime - s_lastTime) * timeScale;
	s_lastTime = s_currentTime;
	//printDeltaTime(); //useful for debug purposes

	/* 
	A few words on this return statement
	Since the very first delta time update returns an extremely high number (due to start up operations)
	The fix I put together checks if the number is unusually high (10 seems enough for any machine)
	If it is that high, it uses recursion to recalculate the value
	*/

	return (s_doubleDeltaTime.count() > 10.0) ? updateDeltaTime() : s_doubleDeltaTime.count(); //this is needed for everything relying on deltaTime on frame 1
};