#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "CubeRobot.h"

class Renderer :
	public OGLRenderer
{
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void UpdateScene(float msec);
	virtual void RenderScene();

protected:
	void DrawNode(SceneNode* n);

	SceneNode* root;
	Camera* camera;
};

