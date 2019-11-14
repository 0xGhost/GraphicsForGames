#pragma once
#include "BoundingVolume.h"

enum BoxCorner {
	BottomLeftNear,
	BottomRightNear,
	BottomRightFar,
	BottomLeftFar,
	TopLeftNear,
	TopRightNear,
	TopRightFar,
	TopLeftFar,
	MaxCornerNum
};

//typedef Vector3 (BoundingBox::*Vector3Function) ();

class BoundingBox :
	public BoundingVolume
{
public:
	BoundingBox(Matrix4 t = Matrix4(), Vector3 o = Vector3(0,0,0)) 
		: BoundingVolume(t, o) {}

	static void CreateBoxMesh()
	{
		OBJMesh* m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"cube.obj");
		box = m;
	}
	static void DeleteBoxMesh() { delete box; }

	virtual bool IsInPlane(Plane p) const;
	virtual void Draw() const override;

protected:
	static OBJMesh *box;
	virtual Vector3 GetMaxDistancePointFromPosition(Vector3 position) const override;
	Vector3 minCorner;
	Vector3 maxCorner;
	
	float* cornerGetter[8][3] = 
	{
		{&minCorner.x, &minCorner.y, &minCorner.z},
		{&maxCorner.x, &minCorner.y, &minCorner.z},
		{&maxCorner.x, &minCorner.y, &maxCorner.z},
		{&minCorner.x, &minCorner.y, &maxCorner.z},
		{&minCorner.x, &maxCorner.y, &minCorner.z},
		{&maxCorner.x, &maxCorner.y, &minCorner.z},
		{&maxCorner.x, &maxCorner.y, &maxCorner.z},
		{&minCorner.x, &maxCorner.y, &maxCorner.z}
	}; 
};

