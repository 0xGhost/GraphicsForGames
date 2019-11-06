#version 330 core

uniform sampler2D diffuseTex;

in Vertex {
	vec4 color;
	vec2 texCoord;
	vec3 	normal;
	vec3 	tangent;
	vec3 	worldPos;
} IN;

out vec4 fragColor;

void main(void){
	vec4 value = texture(diffuseTex, IN.texCoord).rgba;
	if (value.a == 0.0)
	{
		discard;
	}
	fragColor = mix(texture(diffuseTex, IN.texCoord), IN.color, 0.0);
}