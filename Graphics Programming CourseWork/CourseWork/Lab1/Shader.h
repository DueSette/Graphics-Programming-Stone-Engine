#pragma once
#include <string>
#include <GL\glew.h>
#include "Utilities.h"
#include "Transform.h"
#include "Time.h"

class Shader
{
public:
	Shader();
	void createShaderProgram(); //creates shader program without searching for a specific shader (loads the "regular" shaders from file folder)
	void createShaderProgram(const std::string& vertexShaderName, const std::string& fragmentShaderName); //creates shader program and searches for specific shaders

	void Bind(); //Set gpu to use our shaders
	void update(const Transform& transform, const Camera& camera); //feeds shader specific info, called each frame

	std::string Shader::LoadShader(const std::string& fileName);
	void Shader::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
	GLuint Shader::CreateShader(const std::string& text, unsigned int type);

	~Shader();

private:
	static const unsigned int NUM_SHADERS = 2; // number of shaders (vertex and fragment)

	enum
	{
		TRANSFORM_U,

		TIME_U,

		EXTERNAL_COLOR_U,

		NUM_UNIFORMS
	};

	GLuint program; // Track the shader program
	GLuint shaders[NUM_SHADERS]; //array of shaders
	GLuint uniforms[NUM_UNIFORMS]; //no of uniform variables
};
