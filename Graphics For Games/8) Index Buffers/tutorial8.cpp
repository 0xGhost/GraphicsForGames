#include "../../nclgl/window.h"
#include "Renderer.h"

#pragma comment(lib, "nclgl.lib")

int main() {	
	
	//std::cout <<  << endl;

	Window w("Index Buffers!", 800,600,false);
	if(!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}
	return 0;
}