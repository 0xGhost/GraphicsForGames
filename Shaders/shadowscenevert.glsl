#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec3 colour;
in vec3 normal;
in vec3 tangent;
in vec2 texCoord;

out Vertex{
vec3 colour;
vec2 texCoord;
vec3 normal;
vec3 tangent;
vec3 binormal;
vec3 worldPos;
vec4 shadowProj; // New !
} OUT;

void main(void) {
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	OUT.colour = colour;
	OUT.texCoord = texCoord;

	OUT.normal = normalize(normalMatrix * normalize(normal));
	OUT.tangent = normalize(normalMatrix * normalize(tangent));
	OUT.binormal = normalize(normalMatrix *
		normalize(cross(normal, tangent)));

	OUT.worldPos = (modelMatrix * vec4(position, 1)).xyz;
	// New !
	OUT.shadowProj = (textureMatrix * vec4(position + (normal * 15), 1));

	gl_Position = (projMatrix * viewMatrix * modelMatrix) *
		vec4(position, 1.0);
}
