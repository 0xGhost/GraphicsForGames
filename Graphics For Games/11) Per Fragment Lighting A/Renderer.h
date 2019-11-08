#pragma once
#include "..\nclgl\OGLRenderer.h"
#include "..\nclgl\Camera.h"
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
	Mesh* heightMap;
	Camera* camera;
	size_t numberOfLight;
	Light* light[4];
	
};

