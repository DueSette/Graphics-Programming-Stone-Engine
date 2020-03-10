#pragma once
#include <GL/glew.h>
#include <string>
#include <iostream>
#include <stdlib.h>

//Useful macros for catching bugs
#define ASSERT(x) if(!(x)) __debugbreak();

//calls glClearError (improves readability), then calls the function passed in as argument, then uses ASSERT on GLLogCall (which contains a pointer to X function), also triggers a debug break point
#define glCheck(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

#define GRAVITY glm::vec3(0.0f, -9.81f, 0.0f)

//========Useful constants to shorten filepaths and make them less error prone========
static const std::string s_kModels = "..\\res\\models\\";
static const std::string s_kTextures = "..\\res\\textures\\";
static const std::string s_kShaders = "..\\res\\shaders\\";
static const std::string s_kSounds = "..\\res\\audio\\";

//Here are useful functions that are kept in their own file for readability's sake
typedef enum DebugMessageTier { INFO, WARNING, ERROR };
void print(const std::string& message, DebugMessageTier tier);

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);