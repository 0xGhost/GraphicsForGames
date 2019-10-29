#include "../../nclGL/window.h"
#include "Renderer.h"

#pragma comment(lib, "nclgl.lib")

int main() {	
	Window w("Scissors and Stencils!", 800,600,false);	//This is all boring win32 window creation stuff!
	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);	//This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if(!renderer.HasInitialised()) {
		return -1;
	}

	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
			renderer.ToggleScissor();
		}
		if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
			renderer.ToggleStencil();
		}

		renderer.RenderScene();
	}
	return 0;
}