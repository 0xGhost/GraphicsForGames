#pragma once
#include "BoundingBox.h"
class AABoundingBox :
	public BoundingBox
{
public:
	AABoundingBox(Matrix4 t, Vector3 o, Vector3 minCor, Vector3 maxCor) : BoundingBox(t, o), minCorner(minCor), maxCorner(maxCor) {}
	
protected:
	
	Vector3 minCorner;
	Vector3 maxCorner;
};

