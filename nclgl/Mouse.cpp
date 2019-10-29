#include "Mouse.h"

Mouse::Mouse(HWND &hwnd)	{
	ZeroMemory( buttons,	 sizeof(bool) * MOUSE_MAX );
	ZeroMemory( holdButtons, sizeof(bool) * MOUSE_MAX );

	ZeroMemory( doubleClicks,  sizeof(bool)  * MOUSE_MAX );
	ZeroMemory( lastClickTime, sizeof(float) * MOUSE_MAX );

	lastWheel   = 0;
	frameWheel  = 0;
	sensitivity = 0.07f;	//Chosen for no other reason than it's a nice value for my Deathadder ;)
	clickLimit  = 200.0f;

	rid.usUsagePage = HID_USAGE_PAGE_GENERIC; 
    rid.usUsage		= HID_USAGE_GENERIC_MOUSE; 
    rid.dwFlags		= RIDEV_INPUTSINK;   
    rid.hwndTarget	= hwnd;
    RegisterRawInputDevices(&rid, 1, sizeof(rid));
}

void Mouse::Update(RAWINPUT* raw)	{
	if(isAwake)	{
		/*
		Update the absolute and relative mouse movements
		*/
		relativePosition.x +=((float)raw->data.mouse.lLastX ) * sensitivity;
		relativePosition.y +=((float)raw->data.mouse.lLastY ) * sensitivity;

		absolutePosition.x += (float)raw->data.mouse.lLastX;
		absolutePosition.y += (float)raw->data.mouse.lLastY;

		/*
		Bounds check the absolute position of the mouse, so it doesn't disappear off screen edges...
		*/
		absolutePosition.x = max(absolutePosition.x, 0.0f);
		absolutePosition.x = min(absolutePosition.x,absolutePositionBounds.x);

		absolutePosition.y = max(absolutePosition.y, 0.0f);
		absolutePosition.y = min(absolutePosition.y,absolutePositionBounds.y);
	
		/*
		TODO: How framerate independent is this?
		*/
		if(raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL)	{		
			if(raw->data.mouse.usButtonData == 120) {
				frameWheel = 1;
			}
			else {
				frameWheel = -1;
			}
		}

		/*
		Oh, Microsoft...
		*/
		static int buttondowns [5] = {	RI_MOUSE_BUTTON_1_DOWN,
										RI_MOUSE_BUTTON_2_DOWN,
										RI_MOUSE_BUTTON_3_DOWN,
										RI_MOUSE_BUTTON_4_DOWN,
										RI_MOUSE_BUTTON_5_DOWN};

		static int buttonps [5] = {		RI_MOUSE_BUTTON_1_UP,
										RI_MOUSE_BUTTON_2_UP,
										RI_MOUSE_BUTTON_3_UP,
										RI_MOUSE_BUTTON_4_UP,
										RI_MOUSE_BUTTON_5_UP};
		
		for(int i = 0; i < 5; ++i) {
			if(raw->data.mouse.usButtonFlags & buttondowns[i])	{
				//The button was pressed!
				buttons[i] = true;

				/*
				If it wasn't too long ago since we last clicked, we trigger a double click!
				*/
				if(lastClickTime[i] > 0) {
					doubleClicks[i] = true;
				}

				/*
				No matter whether the mouse was double clicked or not, we reset the clicklimit
				*/
				lastClickTime[i] = clickLimit;
			}
			else if(raw->data.mouse.usButtonFlags & buttonps[i])	{
				//The button has been released!
				buttons[i]		= false;
				holdButtons[i]	= false;
			}
		}
	}
}

/*
Sets the mouse sensitivity (higher = mouse pointer moves more!)
Lower bounds checked.
*/
void	Mouse::SetMouseSensitivity(float amount)	{
	if(amount == 0.0f) {
		amount = 1.0f;
	}
	sensitivity = amount;	
}

/*
Updates variables controlling whether a mouse button has been
held for multiple frames. Also updates relative movement.
*/
void Mouse::UpdateHolds()	{
	memcpy(holdButtons,buttons,	MOUSE_MAX * sizeof(bool));
	//We sneak this in here, too. Resets how much the mouse has moved
	//since last update
	relativePosition.ToZero();
	//And the same for the mouse wheel
	frameWheel = 0;
}

/*
Sends the mouse to sleep, so it doesn't process any
movement or buttons until it receives a Wake()
*/
void Mouse::Sleep()	{
	isAwake = false;	//Bye bye for now
	ZeroMemory(holdButtons,  MOUSE_MAX * sizeof(bool) );
	ZeroMemory(buttons,		MOUSE_MAX * sizeof(bool) );
}

/*
Forces the mouse pointer to a specific point in absolute space.
*/
void	Mouse::SetAbsolutePosition(unsigned int x, unsigned int y)	{
	absolutePosition.x = (float)x;
	absolutePosition.y = (float)y;
}

/*
Returns if the button is down. Doesn't need bounds checking - 
an INPUT_KEYS enum is always in range
*/
bool Mouse::ButtonDown(MouseButtons b)	{
	return buttons[b];
}

/*
Returns if the button is down, and has been held down for multiple updates. 
Doesn't need bounds checking - an INPUT_KEYS enum is always in range
*/
bool Mouse::ButtonHeld(MouseButtons b)	{
	return holdButtons[b];
}

/*
Returns how much the mouse has moved by since the last frame.
*/
Vector2	Mouse::GetRelativePosition()	{
	return relativePosition;
}

/*
Returns the mouse pointer position in absolute space.
*/
Vector2 Mouse::GetAbsolutePosition()	{
	return absolutePosition;
}

/*
Returns how much the mouse has moved by since the last frame.
*/
void Mouse::SetAbsolutePositionBounds(unsigned int maxX, unsigned int maxY)	{
	absolutePositionBounds.x = (float)maxX;
	absolutePositionBounds.y = (float)maxY;	
}

/*
Has the mousewheel been moved since the last frame?
*/
bool	Mouse::WheelMoved()	{
	return frameWheel != 0;
};

/*
Returns whether the button was double clicked in this frame
*/
bool Mouse::DoubleClicked(MouseButtons button)	{
	return (buttons[button] && doubleClicks[button]);
}

/*
Get the mousewheel movement. Positive values mean the mousewheel
has moved up, negative down. Can be 0 (no movement)
*/
int		Mouse::GetWheelMovement()	{
	return (int)frameWheel;
}

/*
Updates the double click timers for each mouse button. msec is milliseconds
since the last UpdateDoubleClick call. Timers going over the double click
limit set the relevant double click value to false.
*/
void Mouse::UpdateDoubleClick(float msec)	{
	for(int i = 0; i < MOUSE_MAX; ++i) {
		if(lastClickTime[i] > 0) {
			lastClickTime[i] -= msec;
			if(lastClickTime[i] <= 0.0f) {
				doubleClicks[i]  = false;
				lastClickTime[i] = 0.0f;
			}
		}
	}	
}