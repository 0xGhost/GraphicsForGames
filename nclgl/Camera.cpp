#include "Camera.h"

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::UpdateCamera(float msec)
{
	float mouseDelta = mouseSpeed * msec;

	//Update the mouse by how much
	if (Window::GetMouse()->ButtonHeld(MouseButtons::MOUSE_RIGHT))
	{
		pitch -= (Window::GetMouse()->GetRelativePosition().y) * mouseDelta;
		yaw -= (Window::GetMouse()->GetRelativePosition().x) * mouseDelta;
	}

	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = min(pitch, 90.0f);
	pitch = max(pitch, -90.0f);

	if (yaw < 0)
	{
		yaw += 360.0f;
	}
	if (yaw > 360.0f)
	{
		yaw -= 360.0f;
	}

	float moveDelta = msec * moveSpeed;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) 
	{
		position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * moveDelta;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) 
	{
		position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * moveDelta;
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) 
	{
		position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * moveDelta;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) 
	{
		position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * moveDelta;
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) 
	{
		position.y += moveDelta;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_CONTROL)) 
	{
		position.y -= moveDelta;
	}
}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix() {
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!

	return
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Translation(-position);
		//Matrix4::Scale(Vector3(10.0f,2.0f,5.0f));
};
