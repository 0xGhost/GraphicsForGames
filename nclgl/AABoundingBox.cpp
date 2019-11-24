#include "AABoundingBox.h"
void AABoundingBox::ExpendVolume(BoundingVolume* childBoundingVolume)
{
	Vector3 pointA = childBoundingVolume->GetMaxDistancePointFromPosition(centre + minCorner) - centre;
	maxCorner.x = max(maxCorner.x, pointA.x);
	maxCorner.y = max(maxCorner.y, pointA.y);
	maxCorner.z = max(maxCorner.z, pointA.z);

	Vector3 pointB = childBoundingVolume->GetMaxDistancePointFromPosition(centre + maxCorner) - centre;
	minCorner.x = min(minCorner.x, pointB.x);
	minCorner.y = min(minCorner.y, pointB.y);
	minCorner.z = min(minCorner.z, pointB.z);
}

void AABoundingBox::BoundVolume(BoundingVolume* childBoundingVolume)
{
	maxCorner = childBoundingVolume->GetMaxDistancePointFromPosition(centre + minCorner) - centre;
	minCorner = childBoundingVolume->GetMaxDistancePointFromPosition(centre + maxCorner) - centre;
}

void AABoundingBox::GenerateBoundingVolume(const Mesh& m, Matrix4 modelMatrix)
{
	modelMatrix.values[14] = 0;
	modelMatrix.values[13] = 0;
	modelMatrix.values[12] = 0;

	int size = m.GetNumVertices();
	Vector3 *vertices = m.GetVertices();
	if (!vertices) return;
	minCorner = modelMatrix * vertices[0];
	maxCorner = modelMatrix * vertices[0];

	for (int i = 1; i < size; i++)
	{
		Vector3 temp = modelMatrix * vertices[i];
		maxCorner.x = max(maxCorner.x, temp.x);
		maxCorner.y = max(maxCorner.y, temp.y);
		maxCorner.z = max(maxCorner.z, temp.z);
		minCorner.x = min(minCorner.x, temp.x);
		minCorner.y = min(minCorner.y, temp.y);
		minCorner.z = min(minCorner.z, temp.z);
	}
	//maxCorner = modelMatrix * maxCorner;
	//minCorner = modelMatrix * minCorner;
}

void AABoundingBox::Draw() const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	box->Draw();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

Matrix4 AABoundingBox::GetModelMatrix() const
{
	Vector3 boxCentre = (minCorner + maxCorner) / 2.0f;

	Matrix4 result = transform * Matrix4::Translation(boxCentre);

	result = result * Matrix4::Scale(Vector3(maxCorner.x - minCorner.x + 1,
		maxCorner.y - minCorner.y + 1,
		maxCorner.z - minCorner.z + 1) / 2);
	return result;
}



