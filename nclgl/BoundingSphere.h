#pragma once
#include "BoundingVolume.h"

class BoundingSphere :
	public BoundingVolume
{
public:
	BoundingSphere(Vector3 c, float r): BoundingVolume(c), radius(r){}
	virtual bool IsInPlane(Plane p) const;
	virtual void ExtendVolume(BoundingVolume* childBoundingVolume) override;

protected:

	virtual Vector3 GetMaxDistancePointFromPosition(Vector3 position) const;

	float radius;
};

