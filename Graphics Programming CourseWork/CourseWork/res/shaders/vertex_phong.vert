#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 VertexTexCoord;
layout (location = 2) in vec3 VertexNormal;

out vec3 Position; //position in relation to our view
out vec3 Normal; //direction of the vertex
out vec2 TexCoord; //texture coordinates

uniform mat4 ModelMatrix;
uniform mat4 u_Transform;

//uniform mat3 ViewMatrix;
//uniform mat4 ProjectionMatrix;

void main()
{
	TexCoord = VertexTexCoord;
	Position = vec3(u_Transform * vec4(VertexPosition,1.0));

	Normal = mat3(transpose(inverse(ModelMatrix))) * VertexNormal;

	gl_Position = u_Transform * vec4(VertexPosition,1.0);
}
