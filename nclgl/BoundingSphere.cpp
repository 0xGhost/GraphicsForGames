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

void BoundingSphere::GenerateBoundingVolume(const Mesh& m, Matrix4 modelMatrix)
{
	modelMatrix.values[14] = 0;
	modelMatrix.values[13] = 0;
	modelMatrix.values[12] = 0;

	int size = m.GetNumVertices();
	Vector3* vertices = m.GetVertices();
	radius = 0.0f;

	for (int i = 1; i < size; i++)
	{
		Vector3& temp = vertices[i];
		float length = (temp - centrePosition).Length();
		radius = max(radius, length);
	}
	radius = radius * modelMatrix.GetScalingVector().Length();
}

void BoundingSphere::Update(Matrix4 newTrans)
{
	centrePosition = transform.GetPositionVector();
	transform = newTrans;
}

void BoundingSphere::Draw() const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	sphere->Draw();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

Matrix4 BoundingSphere::GetModelMatrix() const
{
	return transform * Matrix4::Scale(Vector3(radius, radius, radius));
}

Vector3 BoundingSphere::GetMaxDistancePointFromPosition(Vector3 position) const
{
	Vector3 v = centrePosition - position;
	v = v * (v.Length() + radius) / v.Length();
	return v;
}
