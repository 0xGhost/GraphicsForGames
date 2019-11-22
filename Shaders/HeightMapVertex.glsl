#version 150

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform float time;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec3 tangent;
in vec2 texCoord;

out Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} OUT;

void main(void) {

	OUT.colour = colour;
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));


	vec3 newNor = normalize(normalMatrix * normalize(normal));
	newNor.x = newNor.x * min(time, 10000) / 10000;
	newNor.z = newNor.z * min(time, 10000) / 10000;
	OUT.normal = normalize(newNor);// normalize(normalMatrix * normalize(normal));
	
	vec3 newTan = normalize(normalMatrix * normalize(tangent));
	newTan.y = newTan.y * min(time, 10000) / 10000;
	newTan.z = newTan.z * min(time, 10000) / 10000;
	OUT.tangent = normalize(newTan);// normalize(normalMatrix * normalize(tangent));

	vec3 newBin = normalize(normalMatrix * normalize(cross(normal, tangent)));
	newBin.x = newBin.x * min(time, 10000) / 10000;
	newBin.y = newBin.y * min(time, 10000) / 10000;
	OUT.binormal = normalize(newBin);


	vec3 newPos = position;//
	newPos.y = newPos.y * min(time, 10000) / 10000;//

	OUT.worldPos = (modelMatrix * vec4(newPos, 1)).xyz;
	gl_Position = (projMatrix * viewMatrix * modelMatrix) *
		vec4(newPos, 1.0);

}