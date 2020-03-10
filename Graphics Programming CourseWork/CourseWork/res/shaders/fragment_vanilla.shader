#version 430 core

layout(location = 0) in vec2 texCoord0;
layout(location = 0) out vec4 color;

uniform sampler2D u_Diffuse;

void main()
{
	color = texture2D(u_Diffuse, texCoord0);	 //original texture color
}
