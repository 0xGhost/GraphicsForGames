#include "BoundingSphere.h"

bool BoundingSphere::IsInPlane(Plane p) const
{
	return p.SphereInPlane(centrePosition, radius);
}

void BoundingSphere::ExtendVolume(BoundingVolume* childBoundingVolume)
{
	Vector3 point = childBoundingVolume->GetMaxDistancePointFromPosition(centrePosition);
	float distance = (centrePosition - point).Length();
	radius = radius > distance ? radius : distance;
}

Vector3 BoundingSphere::GetMaxDistancePointFromPosition(Vector3 position) const
{
	Vector3 v = centrePosition - position;
	v = v * (v.Length() + radius) / v.Length();
	return Vector3();
}
