#include "BoundingBox.h"

OBJMesh* BoundingBox::box = nullptr;

bool BoundingBox::IsInPlane(Plane p) const
{
	for (int i = 0; i < 8; i++)
	{
		Vector3 temp(*cornerGetter[i][0], *cornerGetter[i][1], *cornerGetter[i][2]);
		temp += transform.GetPositionVector();
		if(p.PointInPlane(temp)) return true;
	}
	return false;
}

Vector3 BoundingBox::GetMaxDistancePointFromPosition(Vector3 position) const
{
	float maxDistance = 0;
	int index = 0;
	for (int i = 0; i < 8; i++)
	{
		Vector3 temp(*cornerGetter[i][0], *cornerGetter[i][1], *cornerGetter[i][2]);
		temp += transform.GetPositionVector();

		float distance = (temp - position).Length();
		if (maxDistance < distance)
		{
			maxDistance = distance;
			index = i;
		}
	}
	Vector3 temp(*cornerGetter[index][0], *cornerGetter[index][1], *cornerGetter[index][2]);
	temp += transform.GetPositionVector();
	return temp;
}

void BoundingBox::Draw() const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	box->Draw();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}