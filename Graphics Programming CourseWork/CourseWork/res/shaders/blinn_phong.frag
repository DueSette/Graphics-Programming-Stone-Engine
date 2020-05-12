#version 330 core
in vec3 FragPosition;
in vec3 Normal;
in vec2 TexCoord;
in vec4 fragPosFromLightPerspective;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightnessColor; //this shader will output a second result to the framebuffer (drawing to another texture, of course)

uniform vec3 CameraPosition;

struct DirectionalLight //positionless light that is coming from infinitely far
{
	vec3 position;
	vec3 color;
};

struct PointLight //"lightbulb" light, has a range and falloff logic
{
	vec4 pos;	
	vec3 color;
	float constantFall, linearFall, quadraticFall;
};

struct Material //The material this object is made of
{
	float ambient; //How much ambient light the object catches (approximated from directional light colour). SET THIS BETWEEN 0 AND 1.
	sampler2D diffuse; //Diffuse reflectivity
	sampler2D specular; //Specular reflectivity
	float shininess; // Specular shininess factor
};

uniform DirectionalLight dirLight;
uniform PointLight[3] lights;
uniform Material mat;
uniform sampler2D shadowMap;
uniform float bloomThreshold;

//calculates whether the current fragment is in shadow by using depth testing from the directional light's perspective
float CalculateShadow()
{
	 float shadow = 0.0;

	 //this line returns the coordinates of the fragment position from coord space to clip space (from -w, w) to (-1, 1)
	 vec3 projCoords = fragPosFromLightPerspective.xyz / fragPosFromLightPerspective.w; //this is necessary only for projection perspective, not orthogonal
	 projCoords = projCoords * 0.5 + 0.5; //normalise the depth between 0 and 1

	 if(projCoords.z > 1.0) { return 0.0; } //if the fragment is at more than 1 depth it means it is outside the view's range, so we simply declare it as non shadow

	 float closestDepth = texture(shadowMap, projCoords.xy).r; //samples the closest depth point from the light's perspective
	 float currentDepth = projCoords.z;  //gets the depth of the current fragment

	 //we need to offset the depth of the fragment by an amount that depends on the angle between the light and the direction of the surface to avoid shadow-acne
	 float shadowOffset = max(0.01 * (1.0 - dot(Normal, -normalize(vec3(dirLight.position) - FragPosition))), 0.005);

	 //======= SOFT SHADOWING PROCESS
	 vec2 texelSize = 1.0 / textureSize(shadowMap, 0); //we get the size of ONE texel (divide shadowmap size at mip0 by one)
	 int passes = 0;
	 for(int x = -1; x <= 1; ++x) //the iterator uses 0 as its center point because we use them to offset positions
	 {
		 for(int y = -1; y <= 1; ++y)
		 {
			//we sample the depth map at different positions. then we add the results together. This is used for softer edges.
			float sampledDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - shadowOffset > sampledDepth ? 1.0 : 0.0; //if the current depth is greater than the minimum possible depth, it means the target is in shadow
			passes++;
		 }    
	 }
	 shadow /= passes;

	 //shadow = currentDepth - shadowOffset > closestDepth ? 1.0 : 0.0; HERE FOR DOCUMENTATION PURPOSES, MULTI SAMPLED SHADOWS ARE NOW USED INSTEAD

	 return shadow;
}

vec3 CalculatePointLightContribution(PointLight light) //calculates the impact on the fragment for every light
{
	vec3 texelColor = vec3(texture(mat.diffuse, TexCoord)); //samples texture color in given fragment, used diffuse (and ambient) maps

	float distanceFromLight = length(light.pos.xyz - FragPosition); //distance from light and light attenuation are used to determine the light's impact
	float lightAttenuation = 1.0 / (light.constantFall + light.linearFall * distanceFromLight + light.quadraticFall * (distanceFromLight * distanceFromLight));

	//CALC AMBIENT
	vec3 ambient = light.color * texelColor * mat.ambient;

	//CALC DIFFUSE
    vec3 lightDir = normalize(vec3(light.pos) - FragPosition);
    float diffuseFactor = max(dot(Normal, lightDir), 0.0); //diffusive factor, based on the angle of the light hitting each point on the surface

    vec3 diffuse = diffuseFactor * light.color * texelColor;

	//CALC SPECULAR
	vec3 viewDirection = normalize(CameraPosition - FragPosition);	
	vec3 halfwayVector = normalize(lightDir + viewDirection); //blinn reflection, handles angles superior to 90 degrees
	float specularFactor = pow(max(dot(Normal, halfwayVector), 0.0), 1 * mat.shininess); //value cannot be negative 
	
	vec3 specular = light.color * specularFactor * vec3(texture(mat.specular, TexCoord)); //we sample the specular factor from the specular map, set via uniform
	//Apply attenuation
	ambient *= lightAttenuation;
	diffuse *= lightAttenuation;
	specular *= lightAttenuation;

	return ambient + diffuse + specular;
}

vec3 CalculateDirectionalLightContribution() //There is no light attenuation for directional light
{
	vec3 lightDir = normalize(dirLight.position - FragPosition);

	//AMBIENT
	vec3 ambient = dirLight.color * mat.ambient * vec3(texture(mat.diffuse, TexCoord));
	
    // DIFFUSE
    float diffuseFactor = max(dot(Normal,lightDir), 0.0);

    // SPECULAR
    vec3 viewDirection = normalize(CameraPosition - FragPosition);	
	vec3 halfwayVector = normalize(lightDir + viewDirection); //blinn reflection, handles angles superior to 90 degrees
	float specularFactor = pow(max(dot(Normal, halfwayVector), 0.0), 1 * mat.shininess);

	//COMBINING FACTORS
    vec3 diffuse  = dirLight.color * diffuseFactor * vec3(texture(mat.diffuse, TexCoord));
    vec3 specular = dirLight.color * specularFactor * vec3(texture(mat.specular, TexCoord));

	float shadow = CalculateShadow();
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

void main()
{
	normalize(Normal);

	vec3 result = CalculateDirectionalLightContribution();

	for (int i = 0; i < lights.length; i++)
	{
		if(lights[i].constantFall == 0) {break;} //if we are iterating through an empty light, break
		result += CalculatePointLightContribution(lights[i]);
	}
	
	FragColor = vec4(result, 1.0); //The actual color of the pixel, not to be confused with what's below

	//=====BLOOM PASS (This does NOT apply bloom, it just checks which fragments are eligible for it)

	//we compare the grayscale of the fragment against the "threshold" value for bloom
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if(brightness > bloomThreshold) //if brightness is still above 1.0 (which can be thanks to HDR) we keep the color
        BrightnessColor = vec4(FragColor.rgb, 1.0);
    else //if not we basically "discard" the fragment (only for the second texture target, the actual color is still kept and rendered)
        BrightnessColor = vec4(0.0, 0.0, 0.0, 1.0);
}
