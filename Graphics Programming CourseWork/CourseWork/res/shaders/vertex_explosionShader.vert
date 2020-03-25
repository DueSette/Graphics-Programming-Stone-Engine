//Version Number
#version 400

//The layout qualifers
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 VertexNormal;

//Uniform variable
uniform mat4 u_Transform;

//Passing out the normal and position data
out vec3 v_norm;
out vec4 v_pos; 
out float time;

out block 
{
	vec2 tCrd;
} vs_out;

void main()
{
	//Assigning the normal and position data
	v_norm = VertexNormal;
	v_pos = vec4(VertexPosition, 1.0);

	vs_out.tCrd = texCoords;

	// Sets the position of the current vertex
	gl_Position = u_Transform * vec4(VertexPosition, 1.0);
}