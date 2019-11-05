#include "BoundingBox.h"

bool BoundingBox::IsInPlane(Plane p) const
{
	for (int i = 0; i < MaxCornerNum; i++)
	{
		if(!p.PointInPlane(Vector3(*cornerGetter[i][0], *cornerGetter[i][1], *cornerGetter[i][2]))) return false;
	}
	return true;
}

void BoundingBox::ExpendVolume(BoundingVolume* childBoundingVolume)
{
	Vector3 point = childBoundingVolume->GetMaxDistancePointFromPosition(transform.GetPositionVector());
	maxCorner.x = max(maxCorner.x, point.x);
	maxCorner.y = max(maxCorner.y, point.y);
	maxCorner.z = max(maxCorner.z, point.z);
	minCorner.x = min(minCorner.x, point.x);
	minCorner.y = min(minCorner.y, point.y);
	minCorner.z = min(minCorner.z, point.z);
}

Vector3 BoundingBox::GetMaxDistancePointFromPosition(Vector3 position) const
{
	float maxDistance = 0;
	int index = 0;
	for (int i = 0; i < MaxCornerNum; i++)
	{
		float distance = (Vector3(*cornerGetter[i][0], *cornerGetter[i][1], *cornerGetter[i][2]) - position).Length();
		if (maxDistance < distance)
		{
			maxDistance = distance;
			index = i;
		}
	}
	return Vector3(*cornerGetter[index][0], *cornerGetter[index][1], *cornerGetter[index][2]);
}
