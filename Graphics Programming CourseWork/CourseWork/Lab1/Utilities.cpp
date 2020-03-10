#include "Utilities.h"

void print(const std::string& message, DebugMessageTier tier)
{
	std::cout << "Importance level: [" << tier << "] " << message << std::endl;
}

//We call this before the function we want to "interrogate" for errors.
//What this function does is, it basically runs until it clears all the errors that GL has flagged so far.
void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

//This gets called right after the function that we want to interrogate, so we can make sure that if any message pops up, it's because of that function.
bool GLLogCall(const char* function, const char* file, int line)
{
	bool gotError = true;

	while (GLenum error = glGetError())
	{
		std::cout << "OPENGL ERROR (" << error << "): " << function <<
			" " << file << " at line: " << line << std::endl;
		gotError = false;
	}
	return gotError;
}