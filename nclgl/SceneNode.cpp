#include "SceneNode.h"

SceneNode::~SceneNode(void)
{
	for (size_t i = 0; i < children.size(); i++)
	{
		delete children[i];
	}
}

void SceneNode::AddChild(SceneNode* s)
{
	children.push_back(s);
	s->parent = this;
}

void SceneNode::Update(float msec)
{
	if (parent)
	{
		worldTransform = parent->worldTransform * transform;
	}
	else
	{
		worldTransform = transform;
	}
	size_t size = children.size();
	for (size_t i = 0; i < size; i++)
	{
		children[i]->Update(msec);
	}
}

void SceneNode::Draw(const OGLRenderer& r)
{
	if (mesh) mesh->Draw();
}
