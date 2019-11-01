#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	meshes[0] = Mesh::GenerateQuad();
	meshes[1] = Mesh::GenerateTriangle();

	meshes[0]->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	meshes[1]->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));

	if (!textures[0] || !textures[1]) return;

	positions[0] = Vector3(0, 0, -5);
	positions[1] = Vector3(0, 0, -5);

	currentShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");

	if (!currentShader->LinkProgram()) return;

	usingDepth = false;
	usingAlpha = false;
	blendMode = 0;
	modifyObject = true;

	projMatrix = Matrix4::Perspective(1, 100, (float)width / (float)height, 45);

	init = true;
}

Renderer::~Renderer()
{
	delete[] meshes;
}

void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(currentShader->GetProgram());

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, (float*)& textureMatrix);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float*)& viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)& projMatrix);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);

	for (unsigned int i = 0; i < 2; i++)
	{
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)& Matrix4::Translation(positions[i]));
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glDepthMask(!i);
		meshes[i]->Draw();
	}
	glUseProgram(0);
	SwapBuffers();
}

void Renderer::ToggleObject()
{
	modifyObject = !modifyObject;

}
void Renderer::MoveObject(float by)
{
	positions[(int)modifyObject].z += by;
}

void Renderer::ToggleDepth() 
{
	usingDepth = !usingDepth;
	usingDepth ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

}

void Renderer::ToggleAlphaBlend() 
{
	usingAlpha = !usingAlpha;
	usingAlpha ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
}

void Renderer::ToggleBlendMode() 
{
	blendMode = (blendMode + 1) % 4;

	switch (blendMode) 
	{
	case (0): glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
	case (1): glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR); break;
	case (2): glBlendFunc(GL_ONE, GL_ZERO); break;
	case (3): glBlendFunc(GL_SRC_ALPHA, GL_ONE); break;
	}
}