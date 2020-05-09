#version 420
in vec3 Position;
in vec3 Normal;

layout( location = 0 ) out vec4 FragColor;

uniform samplerCube skyTexture;

uniform vec3 CameraPosition;

void main()
{
	vec3 direction = normalize(Position - CameraPosition);
    vec3 R = reflect(direction, normalize(Normal)); //reflect the direction vector across the vertex normal
    FragColor = vec4(texture(skyTexture, R).rgb, 1.0); //sample cubemap at the reflected point
}