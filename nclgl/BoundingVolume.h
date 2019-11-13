#pragma once
#include "Plane.h"
#include "Matrix4.h"
#include "..\nclgl\OGLRenderer.h"
#include "..\nclgl\OBJMesh.h"

class BoundingVolume
{
public:
 	//BoundingVolume(Vector3 c) : centrePosition(c) {};
	BoundingVolume(Matrix4 t, Vector3 oS) : transform(t), originScale(oS) {};

	virtual void Draw() const = 0;
	virtual bool IsInPlane(Plane p) const = 0;
	virtual void ExpendVolume(BoundingVolume* childBoundingVolume) = 0;
	virtual Vector3 GetMaxDistancePointFromPosition(Vector3 position) const = 0;
	virtual void Update(Matrix4 newTrans) { transform = newTrans; } // with Scale
	//void SetCentrePosition(Vector3 newPosition) { centrePosition = newPosition; }
	//void SetTransform(Matrix4 newTrans) { transform = newTrans; }

protected:
	//Vector3 centrePosition;
	Matrix4 transform;
	const Vector3 originScale;
};

