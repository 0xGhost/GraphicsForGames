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

typedef Vector3 (BoundingBox::*Vector3Function) ();

class BoundingBox :
	public BoundingVolume
{
public:
	BoundingBox(Vector3 c = Vector3(0, 0, 0), Vector3 minCor = Vector3(0,0,0), Vector3 maxCor = Vector3(0, 0, 0)) : BoundingVolume(c), minCorner(minCor), maxCorner(maxCor) { }

	BoundingBox(Vector3 c,Vector3 cors[MaxCornerNum]) : BoundingVolume(c) { std::copy(cors, cors + MaxCornerNum, corners); }
	
	inline virtual void SetCornPosition(BoxCorner corner, Vector3 position) { corners[corner] = position; }
	inline virtual Vector3 GetCornPosition(BoxCorner corner) const { return corners[corner]; }
	inline virtual Vector3 GetCorn(BoxCorner corner);

	virtual bool IsInPlane(Plane p) const;
	virtual void ExtendVolume(BoundingVolume* childBoundingVolume) override;

protected:

	virtual Vector3 GetMaxDistancePointFromPosition(Vector3 position) const;
	Vector3 minCorner;
	Vector3 maxCorner;
	Vector3 corners[MaxCornerNum];
	/*
	//Vector3(BoundingBox::*cornerGetter[MaxCornerNum])() const =
	Vector3Function cornerGetter[MaxCornerNum]
	 =
	{
		&BoundingBox::BottomLeftNearCorner,
		&BottomRightNearCorner,
		&BottomRightFarCorner,
		&BottomLeftFarCorner,
		&TopLeftNearCorner,
		&TopRightNearCorner,
		&TopRightFarCorner,
		&TopLeftFarCorner,
	};
	*/

	float* a[8][3]; // TODO: point to different min or max

	inline Vector3 BottomLeftNearCorner() { return Vector3(minCorner.x, minCorner.y, minCorner.z); }
	inline Vector3 BottomRightNearCorner() { return Vector3(maxCorner.x, minCorner.y, minCorner.z); }
	inline Vector3 BottomRightFarCorner() { return Vector3(maxCorner.x, minCorner.y, maxCorner.z); }
	inline Vector3 BottomLeftFarCorner() { return Vector3(minCorner.x, minCorner.y, maxCorner.z); }
	inline Vector3 TopLeftNearCorner() { return Vector3(minCorner.x, maxCorner.y, minCorner.z); }
	inline Vector3 TopRightNearCorner() { return Vector3(maxCorner.x, maxCorner.y, minCorner.z); }
	inline Vector3 TopRightFarCorner() { return Vector3(maxCorner.x, maxCorner.y, maxCorner.z); }
	inline Vector3 TopLeftFarCorner() { return Vector3(minCorner.x, maxCorner.y, maxCorner.z); }
};

