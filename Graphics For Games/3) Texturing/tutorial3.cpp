#pragma comment(lib, "nclgl.lib")

#include "../../nclGL/window.h"
#include "Renderer.h"

int main() {
	Window w("Texturing!", 800,600,false);	 //This is all boring win32 window creation stuff!
	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);	//This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if(!renderer.HasInitialised()) {
		return -1;
	}
	float scale = 100.0f;
	float rotation = 0.0f;
	Vector3 position(0, 0, -150.0f);
	float fov = 45.0f;


	renderer.SetCamera(new Camera(0.0f, 0.0f, Vector3(0.0f, 0.0f, 5.0f), 1.2f, 0.8f));
	float rotate = 0.0f;
	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_PLUS))
			++scale;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_MINUS))
			--scale;

		if(Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT) ) {
			--rotate;
			renderer.UpdateTextureMatrix(rotate);
		}

		if(Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT) ) {
			++rotate;
			renderer.UpdateTextureMatrix(rotate);
		}

		if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_1) ) {
			renderer.ToggleFiltering();
		}

		if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_2) ) {
			renderer.ToggleRepeating();
		}

		renderer.SetRotation(rotation);
		renderer.SetScale(scale);
		renderer.SetPosition(position);
		renderer.UpdateScene(w.GetTimer()->GetTimedMS() / 10.0f);
		renderer.RenderScene();
	}

	return 0;
}