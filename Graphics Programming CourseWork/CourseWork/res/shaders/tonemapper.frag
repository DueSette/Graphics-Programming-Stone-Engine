#version 330 core

in vec2 TexCoord;

layout (location = 0) out vec4 FragColor; //actual color of the fragment

uniform sampler2D hdrBufferTexture; //this is the color of the actual scene before post processing is performed
uniform sampler2D brightBlurredTexture; //this is the 

uniform float exposure;

void main()
{                     
    vec3 hdrColor = texture(hdrBufferTexture, TexCoord).rgb;

    //======= BLOOM ADDITIVE BLENDING PASS
    vec3 bloomColor = texture(brightBlurredTexture, TexCoord).rgb; //we simply add the bright blurred texture extracted in the previous pass to the frag color
    hdrColor += bloomColor;

    //======= EXPOSURE BASED TONEMAPPING
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure); //we regulate the "intensity" of the scene via this exposure equation

    //======= GAMMA CORRECTION
    const float gamma = 2.2;
    result = pow(result, vec3(1.0 / gamma));

    FragColor = vec4(result, 1.0);
}