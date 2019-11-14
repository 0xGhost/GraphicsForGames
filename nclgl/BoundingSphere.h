#pragma once
#include "BoundingVolume.h"

class BoundingSphere :
	public BoundingVolume
{
public:
	BoundingSphere(Matrix4 t, Vector3 o, float r) : BoundingVolume(t, o), radius(r) {}
	//BoundingSphere(Vector3 c, float r): BoundingVolume(c), radius(r){}
	virtual bool IsInPlane(Plane p) const;
	virtual void ExpendVolume(BoundingVolume* childBoundingVolume) override;
	virtual void Update(Matrix4 newTrans) override;
	virtual void Draw() const override;
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

