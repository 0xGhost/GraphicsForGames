#pragma once
#include "Plane.h"

class BoundingVolume
{
public:
 	BoundingVolume(Vector3 c) : centrePosition(c) {};

	virtual bool IsInPlane(Plane p) const = 0;
	virtual void ExtendVolume(BoundingVolume* childBoundingVolume) = 0;
	virtual Vector3 GetMaxDistancePointFromPosition(Vector3 position) const = 0;

	void SetCentrePosition(Vector3 newPosition) { centrePosition = newPosition; }
protected:

	
	Vector3 centrePosition;

};

