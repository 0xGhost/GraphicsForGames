// Create by Yiang Lu    
// 12/Nov/2019     
//  V V
// (OwO)
//  | |
//  ^ ^

#pragma comment(lib, "nclgl.lib")

#include "../../NCLGL/window.h"
#include "Renderer.h"

int main() {
	Window w("Impressive", 1920, 1200, true);
	if (!w.HasInitialised()) {
		return -1;
	}

	srand((unsigned int)w.GetTimer()->GetMS() * 1000.0f);

	Renderer renderer(w); 
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}

	return 0;
}


