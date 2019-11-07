#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	camera0 = new Camera(-90.0f, -180.0f, Vector3(4000, 10000, 4000));
	camera = new Camera(-90.0f, -180.0f, Vector3(4000, 10000, 4000));
	quad = Mesh::GenerateQuad();
	Vector3 quad0Positions[4] = { Vector3(0.5f, -1.0f, 1.0f),Vector3(0.5f, -0.5f, 1.0f),Vector3(1.0f, -1.0f, 1.0f),Vector3(1.0f, -0.5f, 1.0f) };
	quad0 = Mesh::GenerateQuad(quad0Positions);
	heightMap = new HeightMap(TEXTUREDIR "terrain.raw");
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "dva.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	sceneShader = new Shader(SHADERDIR "TexturedVertex.glsl", SHADERDIR "TexturedFragment.glsl");
	
	//processShader = new Shader(SHADERDIR "TexturedVertex.glsl", SHADERDIR "processfrag.glsl");
	//processShader = new Shader(SHADERDIR "TexturedVertex.glsl", SHADERDIR "edgeFrag.glsl");
	processShader = new Shader(SHADERDIR "TexturedVertex.glsl", SHADERDIR "doubleVisionFrag.glsl");

	if (!processShader->LinkProgram()
		|| !sceneShader->LinkProgram()
		|| !heightMap->GetTexture())
	{
		return;
	}

	SetTextureRepeating(heightMap->GetTexture(), true);

	// Generate our scene depth texture ...
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenTextures(1, &bufferDepthTex0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex0[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex0[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &bufferFBO); // We ’ll render the scene into this
	glGenFramebuffers(1, &bufferFBO0);
	glGenFramebuffers(1, &processFBO); // And do post processing in this

	
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex0[0], 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE
		|| !bufferDepthTex0
		|| !bufferColourTex0[0])
	{
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);



	// We can check FBO attachment success using this command !
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE
		|| !bufferDepthTex
		|| !bufferColourTex[0])
	{
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	init = true;}Renderer ::~Renderer(void)
{
	delete sceneShader;
	delete processShader;
	currentShader = NULL;

	delete heightMap;
	delete quad;
	delete camera;
	delete quad0;
	delete camera0;

	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(2, bufferColourTex0);
	glDeleteTextures(1, &bufferDepthTex0);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &bufferFBO0);
	glDeleteFramebuffers(1, &processFBO);}void Renderer::UpdateScene(float msec)
{
	//camera0->UpdateCamera(msec);
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene()
{
	DrawScene();
	DrawPostProcess();
	PresentScene();
	SwapBuffers();
}

void Renderer::DrawScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	SetCurrentShader(sceneShader);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	UpdateShaderMatrices();

	heightMap->Draw();
	
	viewMatrix = camera0->BuildViewMatrix();
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	SetCurrentShader(sceneShader);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	UpdateShaderMatrices();

	heightMap->Draw();
	
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPostProcess()
{
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(processShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);	for (int i = 0; i < POST_PASSES; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);

		quad->SetTexture(bufferColourTex[0]);
		quad->Draw();
		// Now to swap the colour buffers , and do the second blur pass
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

		quad->SetTexture(bufferColourTex[1]);
		quad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);}void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(sceneShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	quad->SetTexture(bufferColourTex[0]);
	quad->Draw();
	quad0->SetTexture(bufferColourTex0[0]);
	quad0->Draw();

	glUseProgram(0);


}