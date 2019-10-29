#version 150 core

uniform sampler2D diffuseTex;

in Vertex	{
	vec4 colour;
} IN;

out vec4 gl_FragColor;

void main(void)	{
	gl_FragColor = IN.colour;
}