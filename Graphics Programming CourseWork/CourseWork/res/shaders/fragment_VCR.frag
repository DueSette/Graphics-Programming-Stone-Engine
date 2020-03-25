#version 430 core

layout(location = 0) in vec2 texCoord0;

layout(location = 0) out vec4 color;

uniform sampler2D u_Diffuse;
uniform vec4 u_Color;

void main()
{
	color = texture2D(u_Diffuse, texCoord0);	 //original texture color

	//create new color based on fragment position to make a retrofuturistic VCR effect
	vec4 col = vec4(sin(gl_FragCoord.x * u_Color.x / 5) * 0.1f + 0.1f,
		cos(gl_FragCoord.y * u_Color.x / 5) * 0.1f + 0.2f,
		sin(cos(gl_FragCoord.x * 0.1f)),
		1);

	color = mix(color, col, 0.15f); //lerp our to colors it 15% of the way (from A to B)
	color.r += sin(cos(gl_FragCoord.x)) * 50; //makes a 100 float delta based on the frags horizontal position
}
