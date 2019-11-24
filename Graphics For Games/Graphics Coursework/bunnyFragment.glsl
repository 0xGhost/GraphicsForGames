#version 330 core
#define LightNum 1
#define check(a, b) (((a) & (b)) > 0 ? 1 : 0)

const int PointLight = 1;
const int DirectionLight = 1 << 1;
const int SpotLight = 1 << 2;


uniform sampler2D diffuseTex;

uniform vec3 cameraPos;

uniform vec4 lightColour[LightNum];
uniform vec4 lightSpecularColour[LightNum];
uniform vec3 lightPos[LightNum];
uniform float lightRadius[LightNum];
uniform float lightAngle[LightNum];
uniform vec3 lightDir[LightNum];
uniform int lightType[LightNum];

in Vertex
{
	vec4 colour;
	vec3 normal;
	vec3 worldPos;
	float a;
} IN;

out vec4 fragColour;

void main(void)
{
	/*
	vec4 diffuse = vec4(0.9f, 0.9f, 0.9f, IN.a); //fragColour.a = IN.colour.a;
	for (int i = 0; i < LightNum; i++)
	{

		vec3 incident = normalize(lightPos[i] - IN.worldPos) * check(lightType[i], ~DirectionLight);

		float angle = dot(normalize(lightDir[i]), -incident);
		//if ((lightType[i] & SpotLight) != 0 && angle < cos(radians(lightAngle[i]))) continue;


		incident += normalize(-lightDir[i]) * check(lightType[i], DirectionLight);

		float lambert = max(0.0, dot(incident, IN.normal));

		float dist = length(lightPos[i] - IN.worldPos);
		float atten = 1.0 - clamp(dist / lightRadius[i], 0.0, 1.0) * check(lightType[i], ~DirectionLight);

		vec3 viewDir = normalize(cameraPos - IN.worldPos);
		vec3 halfDir = normalize(incident + viewDir);

		float rFactor = max(0.0, dot(halfDir, IN.normal));
		float sFactor = pow(rFactor, 50.0);

		vec3 colour = (diffuse.rgb * lightColour[i].rgb);
		fragColour = vec4(colour * atten * lambert, diffuse.a) * (1.0 / LightNum);
		//fragColour.rgb += (diffuse.rgb * lightColour[i].rgb) * 0.1 * (1.0 / LightNum);
		fragColour.rgb = fragColour.rgb * (((lightType[i] & SpotLight) != 0 && angle < cos(radians(lightAngle[i]))) == false? 1.0f : 0.0f)
			+ (diffuse.rgb * lightColour[i].rgb) * 0.1 * (1.0 / LightNum);
		//fragColour = fragColour * (normalize(lightDir[i]) * IN.worldPos) > lightAngle[i] ? 0:1;
	}*/
	//fragColour.rgb = vec4(IN.normal, 1);
	
	fragColour = vec4(1,1,1,1);

	//fragColour.a = IN.a;
}