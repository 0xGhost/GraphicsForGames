#include "Renderer.h"
Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	heightMap = new HeightMap(TEXTUREDIR "terrain.raw");
	camera = new Camera(-40, 270, Vector3(-2100, 3300, 2000));

	currentShader = new Shader(SHADERDIR "TexturedVertex.glsl", SHADERDIR "TexturedFragment.glsl");

	if (!currentShader->LinkProgram())
	{
		return;
	}

	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "Dva.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!heightMap->GetTexture())
	{
		return;
	}
	SetTextureRepeating(heightMap->GetTexture(), true);


	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	init = true;
}

Renderer ::~Renderer(void)
{
	delete heightMap;
	delete camera;
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);

	heightMap->Draw();

	glUseProgram(0);
	SwapBuffers();

}
