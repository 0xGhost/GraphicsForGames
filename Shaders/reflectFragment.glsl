#version 150 core

#define LightNum 1
#define check(a, b) (((a) & (b)) > 0 ? 1 : 0)

const int PointLight = 1;
const int DirectionLight = 1 << 1;
const int SpotLight = 1 << 2;

uniform sampler2D diffuseTex;
uniform samplerCube cubeTex;
uniform sampler2D bumpTex;
uniform sampler2D glossTex;

uniform vec3 cameraPos;

uniform vec4 lightColour[LightNum];
uniform vec4 lightSpecularColour[LightNum];
uniform vec3 lightPos[LightNum];
uniform float lightRadius[LightNum];
uniform float lightAngle[LightNum];
uniform vec3 lightDir[LightNum];
uniform int lightType[LightNum];

in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {

	vec4 diffuse = texture (diffuseTex, IN.texCoord) * IN.colour;
	mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);
	vec3 normal = normalize(TBN * (texture (bumpTex, IN.texCoord).rgb * 2.0 - 1.0));

	for (int i = 0; i < LightNum; i++)
	{

		vec3 incident = normalize(IN.worldPos - cameraPos);
		float angle = dot(normalize(lightDir[i]), -incident);
		float dist = length(lightPos[i] - IN.worldPos);
		float atten = 0.7 - clamp(dist / lightRadius[i], 0.0, 0.7) * check(lightType[i], ~DirectionLight);
		vec4 reflection = texture (cubeTex,
			reflect(incident, normalize(IN.normal)));

		fragColour = (lightColour[i] * diffuse * atten) * (diffuse + reflection);

		
	}

	/*
	vec4 diffuse = texture (diffuseTex, IN.texCoord) * IN.colour;
	mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);
	vec3 normal = normalize(TBN * (texture (bumpTex, IN.texCoord).rgb * 2.0 - 1.0));

	vec3 incident = normalize(IN.worldPos - cameraPos);
	float dist = length(lightPos - IN.worldPos);
	float atten = 1.0 - clamp(dist / lightRadius, 0.2, 1.0);
	vec4 reflection = texture (cubeTex,
		reflect(incident, normalize(IN.normal)));

	fragColour = (lightColour * diffuse * atten) * (diffuse + reflection);
	*/

}