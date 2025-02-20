#version 150 core
#define LightNum 1
#define check(a, b) (((a) & (b)) > 0 ? 1 : 0)

const int PointLight = 1;
const int DirectionLight = 1<<1;
const int SpotLight = 1<<2;

uniform sampler2D diffuseTex;
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

in Vertex
{
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void)
{
	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);
	vec3 normal = normalize(TBN * (texture (bumpTex, IN.texCoord).rgb * 2.0 - 1.0));
	vec4 gloss = texture (glossTex, IN.texCoord);
	float power = gloss.r *gloss.g* gloss.b;

	for (int i = 0; i < LightNum; i++)
	{
				
		vec3 incident = normalize(lightPos[i] - IN.worldPos) * check(lightType[i], ~DirectionLight);

		float angle = dot(normalize(lightDir[i]), -incident);
		//if((lightType[i] & SpotLight) != 0 && angle < cos(radians(lightAngle[i]))) continue;


		incident += normalize(-lightDir[i]) *check(lightType[i], DirectionLight);

		float lambert = max(0.0, dot(incident, normal));

		float dist = length(lightPos[i] - IN.worldPos);
		float atten = 1.0 - clamp(dist / lightRadius[i], 0.0, 1.0) *check(lightType[i], ~DirectionLight);

		vec3 viewDir = normalize(cameraPos - IN.worldPos);
		vec3 halfDir = normalize(incident + viewDir);

		float rFactor = max(0.0, dot(halfDir, normal));
		float sFactor = pow(rFactor, power * 50.0f);

		vec3 colour = (diffuse.rgb * lightColour[i].rgb);
		colour += (lightSpecularColour[i].rgb * sFactor) * 0.33;
		fragColour = vec4(colour * atten * lambert, diffuse.a) * (1.0 / LightNum);

		fragColour.rgb = fragColour.rgb * (((lightType[i] & SpotLight) != 0 && angle < cos(radians(lightAngle[i]))) == false? 1.0f : 0.0f)
			+ (diffuse.rgb * lightColour[i].rgb) * 0.1 * (1.0 / LightNum);

		//fragColour.rgb += (diffuse.rgb * lightColour[i].rgb) * 0.1 * (1.0 / LightNum);
		//fragColour = fragColour * (normalize(lightDir[i]) * IN.worldPos) > lightAngle[i] ? 0:1;
	}
}