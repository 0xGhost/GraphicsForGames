#version 150
uniform sampler2D diffuseTex;
uniform int useTexture;

in Vertex{
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColor;

void main(void) {
	fragColor = IN.colour;
	vec4 value = texture(diffuseTex, IN.texCoord).rgba;
	if (value.a == 0)
	{
		discard;
	}
	if (useTexture > 0)
	{
		fragColor *= texture(diffuseTex, IN.texCoord);
	}
}