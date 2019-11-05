#include "BoundingBox.h"

inline Vector3 BoundingBox::GetCorn(BoxCorner corner)
{
	
}

bool BoundingBox::IsInPlane(Plane p) const
{
	for (int i = 0; i < MaxCornerNum; i++)
	{
		if(!p.PointInPlane(*cornerGetter[i]())) return false;
	}
	return true;
}
