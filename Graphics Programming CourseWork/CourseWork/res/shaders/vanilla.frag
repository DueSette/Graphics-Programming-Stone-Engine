#version 430 core

layout(location = 0) in vec2 texCoord0;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightnessColor;


uniform sampler2D u_Diffuse;
uniform vec3 inColor;

void main()
{
	FragColor = vec4(inColor, 1.0);

	//============BLOOM PASS (This does NOT apply bloom, it just checks which fragments are eligible for it)
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722)); //we compare the grayscale of the fragment against the "threshold" value for bloom

    if(brightness > 1.0) //if brightness is still above 1.0 (which can be thanks to HDR) we keep the color
        BrightnessColor = vec4(FragColor.rgb, 1.0);
    else //if not we basically "discard" the fragment (only for the second texture target, the actual color is still kept and rendered)
        BrightnessColor = vec4(0.0, 0.0, 0.0, 1.0);
}
