#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D image; //taken from the hdr framebuffer's second color attachment (called BrightnessColor in-shader)
  
uniform bool horizontal; //we use this value to declare which direction to perform the blur
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216); //gaussian weights, used for the gauss curve


//What this shader does is basically take a small portion of the current texel's colour, sample areas around it and then add the colors together
void main()
{             
    vec2 texelSize = 1.0 / textureSize(image, 0); // gets size of single in-texture pixel (texel)
    vec3 result = texture(image, TexCoord).rgb * weight[0]; // current fragment's contribution

    if(horizontal)
    {
        for(int i = 1; i < weight.length; i++)
        {
            result += texture(image, TexCoord + vec2(texelSize.x * i, 0.0)).rgb * weight[i]; //sample areas around the texel in horizontal space
            result += texture(image, TexCoord - vec2(texelSize.x * i, 0.0)).rgb * weight[i]; //same, but in the opposite direction
        }
    }
    else //same as above, but on the positive and negative Y
    {
        for(int i = 1; i < weight.length; i++)
        {
            result += texture(image, TexCoord + vec2(0.0, texelSize.y * i)).rgb * weight[i];
            result += texture(image, TexCoord - vec2(0.0, texelSize.y * i)).rgb * weight[i];
        }
    }

    FragColor = vec4(result, 1.0);
}