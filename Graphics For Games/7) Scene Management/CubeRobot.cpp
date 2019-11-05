#include "CubeRobot.h"
#include "..//..//nclgl/BoundingSphere.h"

Mesh* CubeRobot::cube = NULL;

CubeRobot::CubeRobot(void)
{
	// Optional, uncomment if you want a local origin marker!
	SetMesh(cube);

	SceneNode* body = new SceneNode(cube, Vector4(1, 0, 0, 0.9f)); // Red!
	body->SetModelScale(Vector3(10, 15, 5));
	body->SetTransform(Matrix4::Translation(Vector3(0, 35, 0)));
	AddChild(body);

	head = new SceneNode(cube, Vector4(0, 1, 0, 1)); // Green!
	head->SetModelScale(Vector3(5, 5, 5));
	head->SetTransform(Matrix4::Translation(Vector3(0, 30, 0)));
	body->AddChild(head);

	leftArm = new SceneNode(cube, Vector4(0, 0, 1, 0.5f)); // Blue!
	leftArm->SetModelScale(Vector3(3, -18, 3));
	leftArm->SetTransform(Matrix4::Translation(Vector3(-12, 30, -1)));
	body->AddChild(leftArm);

	rightArm = new SceneNode(cube, Vector4(0, 0, 1, 0.5f)); // Blue!
	rightArm->SetModelScale(Vector3(3, -18, 3));
	rightArm->SetTransform(Matrix4::Translation(Vector3(12, 30, -1)));
	body->AddChild(rightArm);

	SceneNode* leftLeg = new SceneNode(cube, Vector4(0, 0, 1, 0.3f)); // Blue!
	leftLeg->SetModelScale(Vector3(3, -17.5, 3));
	leftLeg->SetTransform(Matrix4::Translation(Vector3(-8, 0, 0)));
	body->AddChild(leftLeg);

	SceneNode* rightLeg = new SceneNode(cube, Vector4(0, 0, 1, 0.3f)); // Blue!
	rightLeg->SetModelScale(Vector3(3, -17.5, 3));
	rightLeg->SetTransform(Matrix4::Translation(Vector3(8, 0, 0)));
	body->AddChild(rightLeg);

	body->SetBoundingVolume(new BoundingSphere(body->GetWorldTransform(), body->GetModelScale(), 15.0f));
	head->SetBoundingVolume(new BoundingSphere(head->GetWorldTransform(), head->GetModelScale(), 5.0f));
	leftArm->SetBoundingVolume(new BoundingSphere(leftArm->GetWorldTransform(), leftArm->GetModelScale(), 18.0f));
	rightArm->SetBoundingVolume(new BoundingSphere(rightArm->GetWorldTransform(), rightArm->GetModelScale(), 18.0f));
	leftLeg->SetBoundingVolume(new BoundingSphere(leftLeg->GetWorldTransform(), leftLeg->GetModelScale(), 18.0f));
	rightLeg->SetBoundingVolume(new BoundingSphere(rightLeg->GetWorldTransform(), rightLeg->GetModelScale(), 18.0f));

	//body->SetBoundingRadius(15.0f);
	//head->SetBoundingRadius(5.0f);

	//leftArm->SetBoundingRadius(18.0f);
	//rightArm->SetBoundingRadius(18.0f);

	//leftLeg->SetBoundingRadius(18.0f);
	//rightLeg->SetBoundingRadius(18.0f);
}

void CubeRobot::Update(float msec)
{
	transform = transform * Matrix4::Rotation(msec / 10.0f, Vector3(0, 1, 0));
	head->SetTransform(head->GetTransform() * Matrix4::Rotation(-msec / 10.0f, Vector3(0, 1, 0)));
	leftArm->SetTransform(leftArm->GetTransform() * Matrix4::Rotation(msec / 10.0f, Vector3(1, 0, 0)));

	SceneNode::Update(msec);
}
