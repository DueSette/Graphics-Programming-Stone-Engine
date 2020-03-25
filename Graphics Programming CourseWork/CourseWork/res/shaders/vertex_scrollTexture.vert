#version 430 core

layout(location = 0) in vec3 position; //attribute vec3 position;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 texCoord0;

uniform mat4 u_Transform;
uniform float u_Time;

void main()
{
	gl_Position = u_Transform * vec4(position, 1.0);
	
	texCoord0 = texCoord * u_Time;
}
