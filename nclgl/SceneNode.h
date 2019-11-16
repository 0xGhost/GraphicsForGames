#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Mesh.h"
#include "BoundingVolume.h"
#include <vector>

class SceneNode
{
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1), Shader* s = NULL, GLuint t = 0);
	~SceneNode(void);

	void AddChild(SceneNode* s);
	void DeleteChild(size_t index); // index start from 0

	virtual void Update(float msec);
	virtual void Draw(const OGLRenderer& r);
	virtual void ShowBoundingVolume();
	/* TODO: virtual void CalculateBoundingRadius();*/

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() { return children.begin(); }
	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() { return children.end(); }

	void SetTransform(const Matrix4& matrix) { transform = matrix; }
	const Matrix4& GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }
	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }
	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }
	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }
	Shader* GetShader() const { return shader; }
	void SetShader(Shader* s) { shader = s; }
	GLuint GetTexture() const { return texture; }
	void SetTexture(GLuint t) { texture = t; }
	//float GetBoundingRadius() const { return boundingRadius; }
	//void SetBoundingRadius(float br) { boundingRadius = br; }
	BoundingVolume* GetBoundingVolume() const { return boundingVolume; }
	void SetBoundingVolume(BoundingVolume* bv) 
	{ 
		if (boundingVolume) delete boundingVolume; 
		boundingVolume = bv; 
		if (mesh) boundingVolume->GenerateBoundingVolume(*mesh, transform * Matrix4::Scale(modelScale));
	}
	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float dis) { distanceFromCamera = dis; }

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b) { return a->distanceFromCamera < b->distanceFromCamera; }

protected:
	SceneNode* parent;
	Mesh* mesh;
	Shader* shader;
	GLuint texture;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	float distanceFromCamera;
	BoundingVolume *boundingVolume;
	std::vector<SceneNode*> children;
};

