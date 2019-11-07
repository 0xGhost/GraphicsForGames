#version 150 core

uniform vec2 pixelSize;
uniform int isVertical;
uniform sampler2D diffuseTex;

in Vertex
{
	vec4 color;
	vec2 texCoord;
} IN;

out vec4 fragColour;

const mat3 gx = mat3(
	-1, 0, 1,
	-2, 0, 2,
	-1, 0, 1
);

const mat3 gy = mat3(
	-1, -2, -1,
	0, 0, 0,
	1, 2, 1
);


void main(void)
{
	float a = texture2D (diffuseTex, IN.texCoord.xy).a;
	mat3 g;
	if (isVertical == 1)
		g = gy;
	else
		g = gx;
	for (int i = 0; i <= 2; i++)
	{
		for (int j = 0; j <= 2; j++)
		{
			vec3 tmp = texture2D (diffuseTex, IN.texCoord.xy + vec2(pixelSize.x * (i-1), pixelSize.y * (j-1))).rgb;
			
			fragColour += vec4(tmp * g[i][j], 0);
		}
	}
	fragColour.a = a;
}