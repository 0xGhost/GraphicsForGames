#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();

	void SwitchToPerspective();
	void SwitchToOrthographic();
	virtual void UpdateScene(float msec);

	inline void SetScale(float s) { scale = s; }
	inline void SetRotation(float r) { rotation = r; }
	inline void SetPosition(Vector3 p) { position = p; }
	inline void SetFov(float f) { fov = f; }
	inline void SetCamera(Camera* c) { camera = c; }

protected:
	Mesh* triangle;
	Camera* camera;
	float fov;
	float scale;
	float rotation;
	Vector3 position;
};
