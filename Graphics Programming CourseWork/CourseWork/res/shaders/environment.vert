#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 VertexTexCoord;
layout (location = 2) in vec3 VertexNormal;

out vec3 Position; //position in relation to our view
out vec3 Normal; //direction of the vertex

uniform mat4 ModelMatrix;

uniform mat4 ViewProjectionMatrix;

void main()
{
	Normal = mat3(transpose(inverse(ModelMatrix))) * VertexNormal;
    Position = vec3(ModelMatrix * vec4(VertexPosition, 1.0));

    gl_Position = ViewProjectionMatrix * vec4(Position, 1.0);
}
