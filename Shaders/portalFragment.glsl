#version 150 core

uniform samplerCube cubeTex2;

uniform vec3 cameraPos;


in Vertex{
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {


	vec3 incident = normalize(IN.worldPos - cameraPos);

	vec4 reflection = texture (cubeTex2,
		reflect(incident, normalize(IN.normal)));

	fragColour = vec4(1, 1, 1, 1) * reflection;
	fragColour = texture (cubeTex2,
		incident + normalize(IN.normal) * 0.7f ) ;

}