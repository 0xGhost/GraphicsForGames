#version 150
uniform sampler2D diffuseTex;
uniform float mixF = 0.5;

in Vertex {
	vec2 texCoord;
	vec4 color;
} IN;

out vec4 gl_FragColor;

void main(void){
	gl_FragColor = mix(texture(diffuseTex, IN.texCoord), IN.color, mixF);
}