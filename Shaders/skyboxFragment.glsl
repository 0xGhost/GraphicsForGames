#version 150 core

uniform samplerCube cubeTex;
uniform vec3 cameraPos;

in Vertex{
	vec3 normal;
} IN;

out vec4 fragColour;

void main(void) 
{
	fragColour = texture (cubeTex, normalize(IN.normal));
}