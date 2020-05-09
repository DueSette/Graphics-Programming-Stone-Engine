#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D image; //originally from the HDR framebuffer's second color attachment (called BrightnessColor in-shader)
uniform bool horizontal; //we use this value to declare which direction to perform the blur
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162); //gaussian weights, used for the gauss curve

//SHADER EXPLANATION
//What this shader does is basically take a small portion of the current texel's colour, sample areas around it and then add the colors together
//The way this final color is obtained consists in using a 1D kernel, the sum of whose values add up to our standard deviation of 1 (white)
//Having the shader differentiate between the horizontal and vertical passes makes it a lot lighter on the GPU
//Because we don't have to nest the for-loops, so we go from running this 9*9 times to 9+9 times
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
