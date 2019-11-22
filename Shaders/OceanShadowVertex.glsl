#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform float time;

in vec3 position;

void main(void) 
{	
	vec3 newPos = position;//

	newPos.y = newPos.y + (sin(time / 1000 + (newPos.x) / 300) - cos(2 * (time / 1000 + (newPos.x) / 300))) * 60
		+ (sin(time / 1000 + (newPos.z) / 500) - cos(2 * (time / 1000 + (newPos.z) / 500))) * 60;//

	gl_Position = (projMatrix * viewMatrix * modelMatrix) *
		vec4(newPos, 1.0);
}