#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform float time;

in vec3 position;

void main(void) 
{
	vec3 newPos = position;//
	newPos.y = newPos.y * min(time, 10000) / 10000;//
	gl_Position = (projMatrix * viewMatrix * modelMatrix) *
		vec4(newPos, 1.0);
}