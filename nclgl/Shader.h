#pragma once
#include "OGLRenderer.h"

#define SHADER_VERTEX 0
#define SHADER_FRAGMENT 1
#define SHADER_GEOMETRY 2

using namespace std;

class Shader
{
public:
	Shader(string vertec, string fragment, string geometry = "");
	~Shader();
	bool IsLoadFail() { return loadFailed; }
	GLuint GetProgram() { return program; }
	bool LinkProgram();
protected:
	void SetDefaultAttributes();
	bool LoadShaderFile(string from, string& into);
	GLuint GenerateShader(string from, GLenum type);

	GLuint objects[3];
	GLuint program;

	bool loadFailed;
};

