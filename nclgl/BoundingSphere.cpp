#include "BoundingSphere.h"

OBJMesh* BoundingSphere::sphere = nullptr;


bool BoundingSphere::IsInPlane(Plane p) const
{
	return p.SphereInPlane(centrePosition, radius);
}

void BoundingSphere::ExpendVolume(BoundingVolume* childBoundingVolume)
{
	Vector3 point = childBoundingVolume->GetMaxDistancePointFromPosition(centrePosition);
	float distance = (centrePosition - point).Length();
	radius = radius > distance ? radius : distance;
}

void BoundingSphere::Update(Matrix4 newTrans)
{
	BoundingVolume::Update(newTrans);
	centrePosition = transform.GetPositionVector();
	radius *= transform.GetScalingVector().Length() / originScale.Length();
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
