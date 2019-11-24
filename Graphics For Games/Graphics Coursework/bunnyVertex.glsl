#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform vec4 nodeColour;

in vec3 position;
in vec4 colour;
in vec3 normal;

out Vertex
{
	vec4 colour;
	vec3 normal;
	vec3 worldPos;
} OUT;

void main(void)
{
	OUT.colour = colour;

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	OUT.normal = normalize(normalMatrix * normalize(normal));
	//OUT.normal = normalize(vec3(projMatrix * vec4(normalMatrix * normal, 1.0)));
	OUT.worldPos = (modelMatrix * vec4(position, 1)).xyz;
	gl_Position = (projMatrix * viewMatrix * modelMatrix) *
		vec4(position, 1.0);
}
