#pragma once
#include "..\nclgl\OGLRenderer.h"
class Renderer :
	public OGLRenderer
{
public:
	Renderer(Window& parent);
	virtual ~Renderer();

	virtual void RenderScene();

	void ToggleObject();
	void ToggleDepth();
	void ToggleAlphaBlend();
	void ToggleBlendMode();
	void MoveObject(float by);

protected:
	GLuint textures[2];
	Mesh* meshes[2];
	Vector3 positions[2];

	Matrix4 textureMatrix;
	Matrix4 modelMatrix;
	Matrix4 projMatrix;
	Matrix4 viewMatrix;

	bool modifyObject;
	bool usingDepth;
	bool usingAlpha;
	int blendMode;
};

