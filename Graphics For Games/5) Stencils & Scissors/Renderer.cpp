#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	quad = Mesh::GenerateQuad();
	triangle = Mesh::GenerateTriangle();

	currentShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"StencilFragment.glsl");

	if (!currentShader->LinkProgram()) return;

	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"chessboard.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	triangle->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	if (!triangle->GetTexture() || !quad->GetTexture()) return;

	usingScissor = false;
	usingStencil = false;

	//projMatrix = Matrix4::Perspective(1, 100, (float)width / (float)height, 45);

	init = true;
}

Renderer::~Renderer()
{
	delete quad;
	delete triangle;
}

void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if (usingScissor)
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor((float)width / 2.5f, (float)height / 2.5f, (float)width / 5.0f, (float)height / 5.0f);
	}

	glUseProgram(currentShader->GetProgram());

	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	
	if (usingStencil)
	{
		glEnable(GL_STENCIL_TEST);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glStencilFunc(GL_ALWAYS, 2, ~0);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

		quad->Draw();

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilFunc(GL_EQUAL, 2, ~0);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}
	triangle->Draw();
	
	glUseProgram(0);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_STENCIL_TEST);

	SwapBuffers();
}

void Renderer::ToggleScissor() 
{
	usingScissor = !usingScissor;
}

void Renderer::ToggleStencil() 
{
	usingStencil = !usingStencil;
}