/******************************************************************************
Class:GameTimer
Author:Rich Davison
Description:Wraps Windows PerformanceCounter. GameTimers keep track of how much
time has passed since they were last polled - so you could use multiple
GameTimers to trigger events at different time periods. 

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Windows.h"

class GameTimer	{
public:
	GameTimer(void);
	~GameTimer(void) {}

	//How many milliseconds have passed since the GameTimer was created
	float	GetMS();

	//How many milliseconds have passed since GetTimedMS was last called
	float	GetTimedMS();

protected:
	LARGE_INTEGER	start;			//Start of timer
	LARGE_INTEGER	frequency;		//Ticks Per Second

	float lastTime;					//Last time GetTimedMS was called
};

