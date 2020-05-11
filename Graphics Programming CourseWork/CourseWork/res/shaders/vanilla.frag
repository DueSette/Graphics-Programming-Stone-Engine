#version 430 core

layout(location = 0) in vec2 texCoord0;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightnessColor;


uniform sampler2D u_Diffuse;
uniform vec3 inColor;

void main()
{
	FragColor = vec4(inColor, 1.0);
	BrightnessColor = vec4(FragColor.rgb, 1.0); //Does not use threshold for bloom, because this shader is only attached to the lamps which should always bleed light
}
