#pragma once
#include "BoundingBox.h"
class AABoundingBox :
	public BoundingBox
{
public:
	AABoundingBox(Vector3 c, Vector3 minCor, Vector3 maxCor) : BoundingBox(c), minCorner(minCor), maxCorner(maxCor) {}
	virtual bool IsInPlane(Plane p) const;
	virtual void ExtendVolume(BoundingVolume* childBoundingVolume) override;
protected:
	virtual Vector3 GetMaxDistancePointFromPosition(Vector3 position) const;

	Vector3 minCorner;
	Vector3 maxCorner;
};

