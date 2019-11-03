#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Mesh.h"
#include <vector>

class SceneNode
{
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1)) : mesh(m), colour(colour), parent(NULL), modelScale(Vector3(1, 1, 1)) {}
	~SceneNode(void);

	void SetTransform(const Matrix4& matrix) { transform = matrix; }
	const Matrix4& GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }

	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }

	void AddChild(SceneNode* s);

	virtual void Update(float msec);
	virtual void Draw(const OGLRenderer& r);	std::vector<SceneNode*>::const_iterator GetChildIteratorStart()
	{
		return children.begin();
	}

	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd()
	{
		return children.end();	}

protected:
	SceneNode* parent;
	Mesh* mesh;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	std::vector<SceneNode*> children;

};

