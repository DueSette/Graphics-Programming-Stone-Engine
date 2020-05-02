#version 420
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout( location = 0 ) out vec4 FragColor;

uniform vec3 CameraPosition;

struct Light //The light that is interacting with this object
{
	vec4 pos;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constantFall;
	float linearFall;
	float quadraticFall;
};

struct Material //The material this object is made of
{
	sampler2D diffuse; //Diffuse reflectivity
	sampler2D specular; //Specular reflectivity

	float shininess; // Specular shininess factor
};

uniform Light light;
uniform Material mat;

void phongModel(vec4 pos, vec3 norm, out vec3 ambient, out vec3 diffuse, out vec3 specular )
{
	vec3 texelColor = vec3(texture(mat.diffuse, TexCoord)); //samples texture color in given fragment, used diffuse (and ambient) maps
	float distanceFromLight = length(light.pos.xyz - Position);
	float lightAttenuation = 1.0 / (light.constantFall + light.linearFall * distanceFromLight + light.quadraticFall * (distanceFromLight * distanceFromLight));

	//CALC AMBIENT
	ambient = texelColor * light.ambient;

	//CALC DIFFUSE
	vec3 vertexNormal = normalize(norm);
    vec3 lightDir = normalize(vec3(light.pos) - Position);
    float diffuseFactor = max(dot(vertexNormal, vec3(light.pos)), 0.0); //diffusive factor, based on the angle of the light hitting each point on the surface

    diffuse = diffuseFactor * light.diffuse * texelColor;

	//CALC SPECULAR
	vec3 viewDirection = normalize(CameraPosition - Position);	
	vec3 halfwayVector = normalize(lightDir + viewDirection); //blinn reflection, handles angles superior to 90 degrees
	float specularFactor = pow(max(dot(vertexNormal, halfwayVector), 0.0), 1 * mat.shininess);
	
	specular = light.specular * specularFactor * vec3(texture(mat.specular, TexCoord));

	//Apply attenuation
	ambient *= lightAttenuation;
	diffuse *= lightAttenuation;
	specular *= lightAttenuation;
}

void main()
{
	vec3 ambient, diffuse, specular;

	phongModel(light.pos, Normal, ambient, diffuse, specular);

	vec3 result = ambient + diffuse + specular;

	FragColor = vec4(result, 1.0);
}