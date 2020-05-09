#version 330 core

layout(location = 0) out vec4 fragColor;

in vec2 coords;
uniform sampler2D samp;


void main()
{
	fragColor = texture(samp, coords);
}