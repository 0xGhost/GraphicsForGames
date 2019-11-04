#pragma once
#include "Plane.h"
#include "Matrix4.h"
#include "SceneNode.h"
class Matrix4; 
class Frustum
{
public:
	Frustum(){}
	~Frustum(){}
	void FromMatrix(const Matrix4& mvp);
	bool InsideFrustum(SceneNode& n);
protected:
	Plane planes[6];
};

