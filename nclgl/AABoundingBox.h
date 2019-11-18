#pragma once
#include "BoundingBox.h"
class AABoundingBox :
	public BoundingBox
{
public:
	AABoundingBox(Matrix4 t, Vector3 o, Mesh* m = nullptr) : BoundingBox(t, o) 
	{ 
		if (m) GenerateBoundingVolume(*m, t * Matrix4::Scale(o));
		else
		{
			minCorner = Vector3(-1, -1, -1);
			maxCorner = Vector3(1, 1, 1);
		}
	}
	virtual void ExpendVolume(BoundingVolume* childBoundingVolume) override;
	virtual void GenerateBoundingVolume(const Mesh& m, Matrix4 modelMatrix) override;
	virtual void Update(Matrix4 newTrans) override { centre = transform.GetPositionVector(); transform = Matrix4::Translation(centre); } // without scale
	void SetCentre(Vector3 centre) { centre = centre; transform = Matrix4::Translation(centre);
	}
	Vector3 GetCentre() const { return centre; }
	void SetCorner(Vector3 min, Vector3 max) { minCorner = min; maxCorner = max; }
	void Draw() const;
	Matrix4 GetModelMatrix() const override;

protected:
	Vector3 centre;
};

