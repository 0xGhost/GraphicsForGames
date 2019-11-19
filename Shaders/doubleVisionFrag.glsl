#version 150 core

uniform vec2 pixelSize;
uniform int isVertical;
uniform sampler2D diffuseTex;
uniform float offsetNum;

in Vertex
{
	vec4 color;
	vec2 texCoord;
	vec3 	normal;
	vec3 	tangent;
	vec3 	worldPos;
} IN;

out vec4 fragColour;


void main(void)
{

	vec2 offset;
	if (isVertical == 1)
		offset = vec2(-5.0, 0);
	else
		offset = vec2(0, 5.0);

	vec4 temp = texture2D (diffuseTex, IN.texCoord.xy + vec2(offset.x * pixelSize.x, offset.y * pixelSize.y));
	fragColour = texture2D(diffuseTex, IN.texCoord.xy) * 0.5 + temp * 0.5;
}