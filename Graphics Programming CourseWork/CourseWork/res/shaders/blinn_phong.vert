#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 VertexTexCoord;
layout (location = 2) in vec3 VertexNormal;

out vec3 FragPosition; //position in relation to our view
out vec3 Normal; //direction of the vertex
out vec2 TexCoord; //texture coordinates
out vec4 fragPosFromLightPerspective; //the position of the fragment as seen from the directional light's perspective

uniform mat4 ModelMatrix;
uniform mat4 u_Transform;

uniform mat4 lightPerspectiveMatrix; //we can see the depth of the resulting fragment using this matrix: used for shadows

void main()
{
	TexCoord = VertexTexCoord;
	FragPosition = vec3(u_Transform * vec4(VertexPosition, 1.0));
	
	vec3 x = vec3(ModelMatrix * vec4(VertexPosition, 1.0));
	fragPosFromLightPerspective = lightPerspectiveMatrix * vec4(x, 1.0); //fragment position rendered from the perspective of the LIGHT and not the camera

	Normal = mat3(transpose(inverse(ModelMatrix))) * VertexNormal;
	gl_Position = u_Transform * vec4(VertexPosition,1.0);
}
