#pragma once

#include "../NCLGL/OGLRenderer.h"
#include "../NCLGL/Camera.h"
#include "textmesh.h"

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

/*
Draws the passed in line of text, at the passed in position. Can have an optional font size, and
whether to draw it in orthographic or perspective mode.
*/
	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);

protected:
	Camera* camera;		//A camera!
	Font*	basicFont;	//A font! a basic one...
};

