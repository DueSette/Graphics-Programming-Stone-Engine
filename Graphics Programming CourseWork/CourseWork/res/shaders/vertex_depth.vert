#version 420
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

//this shader simply uses the light's perspective for its MVP calculations, instead of the camera's perspective that we normally would use
void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}  