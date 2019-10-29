#include "Window.h"
#include "Mouse.h"
#include "Keyboard.h"

Window* Window::window;

Keyboard*Window::keyboard	= NULL;
Mouse*Window::mouse			= NULL;
//GameTimer*Window::timer		= NULL;

Window::Window(std::string title, int sizeX, int sizeY, bool fullScreen)	{
	renderer		= NULL;
	window			= this;
	forceQuit		= false;
	init			= false;
	mouseLeftWindow	= false;
	lockMouse		= false;
	showMouse		= true;

	this->fullScreen = fullScreen;

	size.x = (float)sizeX; size.y = (float)sizeY;

	fullScreen ? position.x = 0.0f : position.x = 100.0f;
	fullScreen ? position.y = 0.0f : position.y = 100.0f;

	HINSTANCE hInstance = GetModuleHandle( NULL );

	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	if(!GetClassInfoEx(hInstance,WINDOWCLASS,&windowClass))	{
		windowClass.cbSize		= sizeof(WNDCLASSEX);
	    windowClass.style		= CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc	= (WNDPROC)WindowProc;
		windowClass.hInstance	= hInstance;
		windowClass.hCursor		= LoadCursor(NULL, IDC_ARROW);
		windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
		windowClass.lpszClassName = WINDOWCLASS;

		if(!RegisterClassEx(&windowClass)) {
			std::cout << "Window::Window(): Failed to register class!" << std::endl;
			return;
		}
	}

	if(fullScreen) {
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared

		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= sizeX;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= sizeY;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= 32;					// Selected Bits Per Pixel
		dmScreenSettings.dmDisplayFrequency = 60;
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;

		if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)	{
			std::cout << "Window::Window(): Failed to switch to fullscreen!" << std::endl;
			return;
		}
	}

	windowHandle = CreateWindowEx(fullScreen ? WS_EX_TOPMOST : NULL,
	WINDOWCLASS,    // name of the window class
	title.c_str(),   // title of the window
	fullScreen ? WS_POPUP|WS_VISIBLE : WS_OVERLAPPEDWINDOW|WS_POPUP|WS_VISIBLE|WS_SYSMENU|WS_MAXIMIZEBOX|WS_MINIMIZEBOX,    // window style
						(int)position.x,	// x-position of the window
                        (int)position.y,	// y-position of the window
                        (int)size.x,		// width of the window
                        (int)size.y,		// height of the window
                        NULL,				// No parent window!
                        NULL,				// No Menus!
                        hInstance,			// application handle
                        NULL);				// No multiple windows!

 	if(!windowHandle) {
		std::cout << "Window::Window(): Failed to create window!" << std::endl;
		return;
	}

	if(!keyboard) {
		keyboard	= new Keyboard(windowHandle);
	}
	if(!mouse) {
		mouse		= new Mouse(windowHandle);
	}
	//if(!timer) {
		timer		= new GameTimer();
	//}
	elapsedMS	= timer->GetMS();

	Window::GetMouse()->SetAbsolutePositionBounds((unsigned int)size.x,(unsigned int)size.y);

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(window->windowHandle, &pt);
	Window::GetMouse()->SetAbsolutePosition(pt.x,pt.y);

	LockMouseToWindow(lockMouse);
	ShowOSPointer(showMouse);

	init = true;
}


Window::~Window(void)
{
	delete keyboard;keyboard = NULL;
	delete mouse;	mouse = NULL;
}

HWND Window::GetHandle() {
	return windowHandle;
}

bool Window::HasInitialised() {
	return init;
}

void	Window::SetRenderer(OGLRenderer* r)	{
	renderer = r;
	if(r) {
		renderer->Resize((int)size.x,(int)size.y);				
	}
}

bool	Window::UpdateWindow() {
	MSG		msg;

	float diff = timer->GetMS()-elapsedMS;

	Window::GetMouse()->UpdateDoubleClick(diff);

	Window::GetKeyboard()->UpdateHolds();
	Window::GetMouse()->UpdateHolds();

	while(PeekMessage(&msg,windowHandle,0,0,PM_REMOVE)) {
		CheckMessages(msg); 
	}

	elapsedMS = timer->GetMS();

	return !forceQuit;
}

void Window::CheckMessages(MSG &msg)	{
	switch (msg.message)	{				// Is There A Message Waiting?
		case (WM_QUIT):
		case (WM_CLOSE): {					// Have We Received A Quit Message?
			window->ShowOSPointer(true);
			window->LockMouseToWindow(false);
			forceQuit = true;
		}break;
		case (WM_INPUT): {
			UINT dwSize;
			GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, NULL, &dwSize,sizeof(RAWINPUTHEADER));

			BYTE* lpb = new BYTE[dwSize];
	
			GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, lpb, &dwSize,sizeof(RAWINPUTHEADER));
			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (keyboard && raw->header.dwType == RIM_TYPEKEYBOARD) {
				Window::GetKeyboard()->Update(raw);
			}

			if (mouse && raw->header.dwType == RIM_TYPEMOUSE) {
				Window::GetMouse()->Update(raw);
			}
			delete lpb;
		}break;

		default: {								// If Not, Deal With Window Messages
			TranslateMessage(&msg);				// Translate The Message
			DispatchMessage(&msg);				// Dispatch The Message
		}
	}
}

LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)	{
    switch(message)	 {
        case(WM_DESTROY):	{
			window->ShowOSPointer(true);
			window->LockMouseToWindow(false);

			PostQuitMessage(0);
			window->forceQuit = true;
		} break;
		case (WM_ACTIVATE): {
			//int fMinimized	= (BOOL) HIWORD(wParam);
			if(LOWORD(wParam) == WA_INACTIVE)	{
				ReleaseCapture();
				ClipCursor(NULL);
				mouse->Sleep();
				keyboard->Sleep();			
			}
			else{
				if(window->init) {
					mouse->Wake();	
					keyboard->Wake();

					POINT pt;
					GetCursorPos(&pt);
					ScreenToClient(window->windowHandle, &pt);
					mouse->SetAbsolutePosition(pt.x,pt.y);

					if(window->lockMouse) {
						window->LockMouseToWindow(true);
					}
				}
			}
			return 0;
		}break;
		case (WM_LBUTTONDOWN): {
			if(window->lockMouse) {
				window->LockMouseToWindow(true);
			}

		}break;

		case (WM_MOUSEMOVE): {
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = window->windowHandle;
			TrackMouseEvent(&tme);

			if(window->mouseLeftWindow) {
				window->mouseLeftWindow = false;
				mouse->Wake();
				keyboard->Wake();

				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(window->windowHandle, &pt);
				mouse->SetAbsolutePosition(pt.x,pt.y);
			}

		}break;
		case(WM_MOUSELEAVE):{
			window->mouseLeftWindow = true;
			mouse->Sleep();
			keyboard->Sleep();
		}break;
		case(WM_SIZE): {
			window->size.x = (float)LOWORD(lParam);
			window->size.y = (float)HIWORD(lParam);
			if(window->renderer) {
				window->renderer->Resize(LOWORD(lParam),HIWORD(lParam));				
			}

			if(window->init) {
				mouse->SetAbsolutePositionBounds(LOWORD(lParam),HIWORD(lParam));

				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(window->windowHandle, &pt);
				mouse->SetAbsolutePosition(pt.x,pt.y);

				window->LockMouseToWindow(window->lockMouse);
			}
		}break;
    }
    return DefWindowProc (hWnd, message, wParam, lParam);
}

void	Window::LockMouseToWindow(bool lock)	{
	lockMouse = lock;
	if(lock) {
		RECT		windowRect;
		GetWindowRect (window->windowHandle, &windowRect);

		SetCapture(window->windowHandle);
		ClipCursor(&windowRect);

		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(window->windowHandle, &pt);
		Window::GetMouse()->SetAbsolutePosition(pt.x,pt.y);
	}
	else{
		ReleaseCapture();
		ClipCursor(NULL);
	}
}

void	Window::ShowOSPointer(bool show)	{
	if(show == showMouse) {
		return;	//ShowCursor does weird things, due to being a counter internally...
	}

	showMouse = show;
	if(show) {
		ShowCursor(1);
	}
	else{
		ShowCursor(0);
	}
}