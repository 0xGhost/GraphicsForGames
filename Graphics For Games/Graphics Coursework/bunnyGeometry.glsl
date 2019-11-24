#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;
uniform mat4 modelMatrix;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

const float MAGNITUDE = 40;

in Vertex
{
	vec4 colour;
	  vec3 normal;
	  vec3 worldPos;
} IN[];

out Vertex
{
	vec4 colour;
	 vec3 normal;
	 vec3 worldPos;
	 float a;
	 //flat float fur_strength;
} OUT;

void GenerateLine(int index)
{
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();
	gl_Position.xyz = gl_in[index].gl_Position.xyz + normalize(projMatrix * vec4(IN[index].normal, 1.0)).xyz * MAGNITUDE;
	EmitVertex();
	EndPrimitive();
}

void main(void)
{
	GenerateLine(0); // first vertex normal
	GenerateLine(1); // second vertex normal
	GenerateLine(2); // third vertex normal

};
