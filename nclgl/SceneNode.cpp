#include "SceneNode.h"
#include "BoundingSphere.h"
#include "AABoundingBox.h"

SceneNode::SceneNode(Mesh* m, Vector4 colour, Shader* s, GLuint t) 
	: mesh(m), shader(s), texture(t), colour(colour), parent(NULL), modelScale(Vector3(1, 1, 1))
{

	if (t) mesh->SetTexture(t);
	boundingVolume = new AABoundingBox(worldTransform, modelScale);
	if (mesh) boundingVolume->GenerateBoundingVolume(*mesh, transform * Matrix4::Scale(modelScale));
	distanceFromCamera = 0.0f;
}

SceneNode::~SceneNode(void)

{
	for (size_t i = 0; i < children.size(); i++)
	{
		delete children[i];
	}
	if (shader && !shader->IsLoadFail()) delete shader;
	delete boundingVolume;
}

void SceneNode::AddChild(SceneNode* s)
{
	if (s == this) return;
	children.push_back(s);
	s->parent = this;
	//boundingVolume->ExpendVolume(s->boundingVolume);
}

void SceneNode::DeleteChild(size_t index)
{
	delete children[index];
	children.erase(children.begin() + index);
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
	
	boundingVolume->Update(worldTransform);
	//boundingVolume->SetCentrePosition(worldTransform.GetPositionVector());
	size_t size = children.size();
	if (size > 0)
	{
		boundingVolume->BoundVolume(children[0]->boundingVolume);
		for (size_t i = 0; i < size; i++)
		{
			children[i]->Update(msec);
			boundingVolume->ExpendVolume(children[i]->boundingVolume);
		}
	}
}

void SceneNode::Draw(const OGLRenderer& r)
{
	if (mesh) mesh->Draw();
}

void SceneNode::ShowBoundingVolume()
{
	if (boundingVolume) boundingVolume->Draw();
}


