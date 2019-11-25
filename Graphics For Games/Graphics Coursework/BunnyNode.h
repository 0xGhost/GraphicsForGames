#pragma once
#include "..\nclgl\SceneNode.h"
class BunnyNode :
	public SceneNode
{
public:
	virtual void Draw(const OGLRenderer& r) 
	{ 
		//glDepthMask(GL_FALSE); 
		SceneNode::Draw(r); 
		//glDepthMask(GL_TRUE);
	}

};

