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
	float maxX = 0.0f;
	float maxY = 0.0f;
	float maxZ = 0.0f;
	int maxXIndex = 0;
	int maxYIndex = 0;
	int maxZIndex = 0;

	for (int i = 1; i < size; i++)
	{
		radius = max(radius, (modelMatrix * vertices[i]).Length());
	}
	/*
		Vector3& temp = vertices[i];

		if (maxX < abs(temp.x))
		{
			maxXIndex = i;
			maxX = abs(temp.x);
		}
		if (maxY < abs(temp.y))
		{
			maxXIndex = i;
			maxY = abs(temp.y);
		}
		if (maxZ < abs(temp.z))
		{
			maxZIndex = i;
			maxZ = abs(temp.z);
		}
	}
	radius = max(vertices[maxXIndex].Length(), max(vertices[maxYIndex].Length(), vertices[maxZIndex].Length()));
	/*
	Vector3 maxVolume(maxX, maxY, maxZ);
	maxVolume = modelMatrix * maxVolume;
	radius = max(maxVolume.x, max(maxVolume.y, maxVolume.z));*/
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
	float temp = (v.Length() + radius) / v.Length();
	v = v * temp;
	return v + centrePosition;
}
