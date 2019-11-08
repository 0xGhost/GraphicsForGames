#version 150 core

uniform sampler2D diffuseTex;

uniform vec3 cameraPos;

uniform vec4 lightColour[4];
uniform vec4 lightSpecularColour[4];
uniform vec3 lightPos[4];
uniform float lightRadius[4];

in Vertex
{
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void)
{
	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	for (int i = 0; i < 4; i++)
	{
		vec3 incident = normalize(lightPos[i] - IN.worldPos);
		float lambert = max(0.0, dot(incident, IN.normal));

		float dist = length(lightPos[i] - IN.worldPos);
		float atten = 1.0 - clamp(dist / lightRadius[i], 0.0, 1.0);

		vec3 viewDir = normalize(cameraPos - IN.worldPos);
		vec3 halfDir = normalize(incident + viewDir);

		float rFactor = max(0.0, dot(halfDir, IN.normal));
		float sFactor = pow(rFactor, 50.0);

		vec3 colour = (diffuse.rgb * lightColour[i].rgb);
		colour += (lightSpecularColour[i].rgb * sFactor);// * 0.33;
		fragColour = vec4(colour * atten * lambert, diffuse.a) * 0.25;
		fragColour.rgb += (diffuse.rgb * lightColour[i].rgb) * 0.1 * 0.25;
	}
}