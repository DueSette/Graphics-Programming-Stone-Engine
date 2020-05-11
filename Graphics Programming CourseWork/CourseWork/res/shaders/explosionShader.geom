//Version number
#version 400

//Layout qualfier
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

//Passing in texture coordinates
in block {
    vec2 tCrd;
} gs_in[];

out vec2 coords;

//Uniform variabe
uniform float u_Time;

vec4 explode(vec4 position, vec3 normal)
{
    //Amout of explosion
    float magnitude = 8.0;
	//Direction of explosion, going along normal
    vec3 direction = normal * ((sin(u_Time) + 1.0) / 2.0) * magnitude; 
	//Returning position
    return position + vec4(direction, 0.0);
}

vec3 GetNormal()
{
//Getting the normal vector of each vertex
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   //returns the cross product between the two vectors calculated
   return normalize(cross(a, b));
}

void main()
{
//Getting normal
    vec3 normal = GetNormal();

//Setting current vertex position
 for (int i = 0; i < gl_in.length(); i++) 
    {
        gl_Position = explode(gl_in[i].gl_Position, normal);
        coords = gs_in[i].tCrd;
        EmitVertex();
    }

    EndPrimitive();    
}