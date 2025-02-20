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
	vec4 shadowProj; // New !
} OUT;
float rand(float n) { return fract(sin(n) * 43758.5453123); }

float noise(float p) {
	float fl = floor(p);
	float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}

void main(void) {
	OUT.colour = colour;
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	vec3 newPos = position;//

	newPos.y = newPos.y + (sin(time / 1000 + (newPos.x) / 300) - cos(2 * (time / 1000 + (newPos.x) / 300))) * 60
		+ (sin(time / 1000 + (newPos.z) / 500) - cos(2 * (time / 1000 + (newPos.z) / 500))) * 60;//

	float newPosYTan = (sin(time / 1000 + (newPos.x + 1) / 300) - cos(2 * (time / 1000 + (newPos.x + 1) / 300))) * 60
		+ (sin(time / 1000 + (newPos.z) / 500) - cos(2 * (time / 1000 + (newPos.z) / 500))) * 60;//

	float newPosYBin = (sin(time / 1000 + (newPos.x) / 300) - cos(2 * (time / 1000 + (newPos.x) / 300))) * 60
		+ (sin(time / 1000 + (newPos.z - 1) / 500) - cos(2 * (time / 1000 + (newPos.z - 1) / 500))) * 60;//




	vec3 newTan = normalize(normalMatrix * normalize(tangent));
	newTan.y = (newPosYTan - newPos.y);

	vec3 newBin = normalize(normalMatrix * normalize(cross(normal, tangent)));
	newBin.y = (newPosYBin - newPos.y);


	OUT.tangent = normalize(newTan);
	OUT.binormal = normalize(newBin);
	
	OUT.normal = normalize(cross(newTan, newBin));

	


	OUT.shadowProj = (textureMatrix * vec4(newPos + (normal * 15), 1));

	OUT.worldPos = (modelMatrix * vec4(newPos, 1)).xyz;
	gl_Position = (projMatrix * viewMatrix * modelMatrix) *
		vec4(newPos, 1.0);

}