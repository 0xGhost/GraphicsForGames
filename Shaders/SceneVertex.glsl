#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec4 nodeColour;

in vec3 position;
in vec2 texCoord;

out Vertex{
	vec2 texCoord;
	vec4 colour;
} OUT;

void main(void) {
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position, 1.0);
	OUT.texCoord = texCoord;
	OUT.colour = nodeColour;
}