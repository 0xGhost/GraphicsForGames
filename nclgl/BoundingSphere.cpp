#include "BoundingSphere.h"

OBJMesh* BoundingSphere::sphere = nullptr;


bool BoundingSphere::IsInPlane(Plane p) const
{
	return p.SphereInPlane(centrePosition, radius);
}

void BoundingSphere::ExpendVolume(BoundingVolume* childBoundingVolume)
{
	float maxR = 0;
	Vector3 point = childBoundingVolume->GetMaxDistancePointFromPosition(centrePosition);
	float distance = (centrePosition - point).Length();
	maxR = maxR > distance ? maxR : distance;
	radius = maxR;
}

void BoundingSphere::Update(Matrix4 newTrans)
{
	centrePosition = transform.GetPositionVector();
	//radius *= newScale.GetScalingVector().Length() / originScale.Length();
	transform = newTrans * Matrix4::Scale(Vector3(radius, radius, radius));
}

void BoundingSphere::Draw() const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	sphere->Draw();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

Vector3 BoundingSphere::GetMaxDistancePointFromPosition(Vector3 position) const
{
	Vector3 v = centrePosition - position;
	v = v * (v.Length() + radius) / v.Length();
	return v;
}
