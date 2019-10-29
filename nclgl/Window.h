/*
Class:Window
Author:Rich Davison
Description:Creates and handles the Window, including the initialisation of the mouse and keyboard.
*/
#pragma once
#pragma warning( disable : 4099 )

#include <string>

#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>

#include "OGLRenderer.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "GameTimer.h"

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define WINDOWCLASS "WindowClass"

class OGLRenderer;

class Window	{
public:
	Window(std::string title = "OpenGL Framework", int sizeX = 800, int sizeY = 600, bool fullScreen = false);
	~Window(void);

	bool	UpdateWindow();	

	void	SetRenderer(OGLRenderer* r);

	HWND	GetHandle();

	bool	HasInitialised();

	void	LockMouseToWindow(bool lock);
	void	ShowOSPointer(bool show);

	Vector2	GetScreenSize() {return size;};

	static Keyboard*	GetKeyboard()	{return keyboard;}
	static Mouse*		GetMouse()		{return mouse;}

	GameTimer*   GetTimer()		{return timer;}

protected:
	void	CheckMessages(MSG &msg);
	static LRESULT CALLBACK WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

	HWND			windowHandle;

	static Window*		window;
	static Keyboard*	keyboard;
	static Mouse*		mouse;

	GameTimer*	timer;

	OGLRenderer*		renderer;

	bool				forceQuit;
	bool				init;
	bool				fullScreen;
	bool				lockMouse;
	bool				showMouse;

	Vector2				position;
	Vector2				size;

	float				elapsedMS;

	bool				mouseLeftWindow;
};