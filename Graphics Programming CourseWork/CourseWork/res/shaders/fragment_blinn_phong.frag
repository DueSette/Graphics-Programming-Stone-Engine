#version 420
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout( location = 0 ) out vec4 FragColor;

uniform sampler2D texSampler;

uniform vec4 LightPosition; // Light position in eye coords.
uniform vec3 LightColor;
uniform vec3 CameraPosition;

//uniform vec3 Intensity;

uniform vec3 Ka; // Ambient reflectivity
uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ks; // Specular reflectivity
uniform float Shininess; // Specular shininess factor

void phongModel(vec4 pos, vec3 norm, out vec3 ambient, out vec3 diffuse, out vec3 specular )
{
	//CALC AMBIENT
	ambient = Ka * LightColor;

	//CALC DIFFUSE
	vec3 vertexNormal = normalize(norm);
    vec3 lightDir = normalize(vec3(LightPosition) - Position);

    float diff = max(dot(vertexNormal, vec3(LightPosition)), 0.0); //diffusive factor, based on the angle of the light hitting each point on the surface
    diffuse = diff * Kd * LightColor;

	//CALC SPECULAR
	vec3 viewDirection = normalize(CameraPosition - Position);	
	vec3 halfwayVector = normalize(lightDir + viewDirection); //blinn reflection, handles angles superior to 90 degrees
	float spec = pow(max(dot(vertexNormal, halfwayVector), 0.0), 16 * Shininess);
	
	specular = Ks * spec * LightColor;
}

void main()
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec4 texColor = texture2D(texSampler, TexCoord);

	phongModel(LightPosition, Normal, ambient, diffuse, specular);

	vec3 result = (ambient + diffuse + specular) * vec3(texColor);
	FragColor = vec4(result, 1.0);
}