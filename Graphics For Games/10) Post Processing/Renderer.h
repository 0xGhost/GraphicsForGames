#pragma once
#include "..\nclgl\OGLRenderer.h"
#include "..\nclgl\HeightMap.h"
#include "..\nclgl\Camera.h"

#define POST_PASSES 1

class Renderer :
	public OGLRenderer
{
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	void PresentScene();
	void DrawPostProcess();
	void DrawScene();

	Shader* sceneShader;
	Shader* processShader;

	Camera* camera;
	Camera* camera0;

	//Mesh* quadBurrArea;
	Mesh* quad;
	Mesh* quad0;
	HeightMap* heightMap;

	GLuint bufferFBO;
	GLuint bufferFBO0;
	GLuint processFBO;
	GLuint processFBO0;
	GLuint bufferColourTex[2];
	GLuint bufferColourTex0[2];
	GLuint bufferDepthTex;
	GLuint bufferDepthTex0;
};

