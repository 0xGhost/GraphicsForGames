#pragma once
#include "BoundingVolume.h"

class BoundingSphere :
	public BoundingVolume
{
public:
	BoundingSphere(Matrix4 t, Vector3 o, Mesh* m = nullptr) : BoundingVolume(t, o) 
	{
		if (m) GenerateBoundingVolume(*m, t * Matrix4::Scale(o));
		else
		{
			radius = 1;
		}
	}
	//BoundingSphere(Vector3 c, float r): BoundingVolume(c), radius(r){}
	virtual bool IsInPlane(Plane p) const;
	virtual void ExpendVolume(BoundingVolume* childBoundingVolume) override;
	virtual void GenerateBoundingVolume(const Mesh& m, Matrix4 modelMatrix) override;
	virtual void Update(Matrix4 newTrans) override;
	virtual void Draw() const override;
	virtual Matrix4 GetModelMatrix() const override;
	static void CreateSphereMesh()
	{
		OBJMesh* m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"ico.obj");
		sphere = m;
	}
	static void DeleteSphereMesh() { delete sphere; }

protected:
	static OBJMesh *sphere;
	virtual Vector3 GetMaxDistancePointFromPosition(Vector3 position) const;
	Vector3 centrePosition;
	float radius;
};

