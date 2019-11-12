#pragma once

#include "..\nclgl\OGLRenderer.h"
#include "..\nclgl\Camera.h"
#include "..\nclgl\MD5Mesh.h"
#include "..\nclgl\MD5Node.h"

#define SHADOWSIZE 2048 // New !

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void UpdateScene(float msec);
	virtual void RenderScene();

protected:
	void DrawMesh(); // New !
	void DrawFloor(); // New !
	void DrawShadowScene(); // New !
	void DrawCombinedScene(); // New !

	Shader* sceneShader;
	Shader* shadowShader;

	GLuint shadowTex;
	GLuint shadowFBO;

	MD5FileData* hellData;
	MD5Node* hellNode;
	Mesh* floor;
	Camera* camera;
	Light* light;
};
