#pragma once
#include "..\nclgl\OGLRenderer.h"
#include "..\nclgl\camera.h"
#include "..\nclgl\HeightMap.h"


class Renderer :
	public OGLRenderer
{
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	HeightMap* heightMap;
	Camera* camera;
};

