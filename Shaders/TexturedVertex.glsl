#version 150

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec4 color;
in vec2 texCoord;

out Vertex {
	vec4 color;
	vec2 texCoord;
	//vec3 	normal;
	//vec3 	tangent;
	//vec3 	worldPos;
} OUT;

void main(void) {
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position, 1.0);
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
	OUT.color = color;
}