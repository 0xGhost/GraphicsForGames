#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent), fov(45.0f)
{
	triangle = Mesh::GenerateTriangle();
	triangle->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!triangle->GetTexture()) return;

	currentShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	if (!currentShader->LinkProgram()) return; 

	init = true;

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, fov);

	filtering = true;
	repeating = false;
}

void Renderer::UpdateScene(float msec) 
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

Renderer::~Renderer()
{
	delete triangle;
}

void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(currentShader->GetProgram());


		Vector3 tempPos = position;
#if 1
		modelMatrix = Matrix4::Translation(tempPos) *
			Matrix4::Rotation(rotation, Vector3(0, 1, 0)) *
			Matrix4::Scale(Vector3(scale, scale, scale));
#else
		modelMatrix =
			Matrix4::Scale(Vector3(scale, scale, scale)) *
			Matrix4::Rotation(rotation, Vector3(0, 1, 0)) *
			Matrix4::Translation(tempPos);
#endif
		//glUniformMatrix4fv(glGetUniformLocation(
		//	currentShader->GetProgram(), "modelMatrix"), 1, false,
		//	(float*)& modelMatrix);



	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "mixF"), 0.1f);
	triangle->Draw();
	glUseProgram(0);
	SwapBuffers();
}

void Renderer::UpdateTextureMatrix(float r)
{
	Matrix4 pushPos = Matrix4::Translation(Vector3(0.5f, 0.5f, 0));
	Matrix4 popPos = Matrix4::Translation(Vector3(-0.5f, -0.5f, 0));
	Matrix4 rotation = Matrix4::Rotation(r, Vector3(0, 0, 1));
	textureMatrix = pushPos * rotation * popPos;
}

void Renderer::ToggleRepeating()
{
	repeating = !repeating;
	glBindTexture(GL_TEXTURE_2D, triangle->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, //x axis
		repeating ? GL_REPEAT : GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, //y axis
		repeating ? GL_REPEAT : GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::ToggleFiltering()
{
	filtering = !filtering;
	glBindTexture(GL_TEXTURE_2D, triangle->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		filtering ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		filtering ? GL_LINEAR : GL_NEAREST);
	float aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
	glBindTexture(GL_TEXTURE_2D, 0);
}
