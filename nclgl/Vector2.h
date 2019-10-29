#pragma once
/*
Class:Vector2
Implements:
Author:Rich Davison
Description:VERY simple Vector2 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
#include <iostream>

class Vector2	{
public:
	Vector2(void) {
		ToZero();
	}

	Vector2(const float x, const float y) {
		this->x = x;
		this->y = y;
	}

	~Vector2(void){}

	float x;
	float y;

	void ToZero(){
		x = 0.0f;
		y = 0.0f;
	}

	inline friend std::ostream& operator<<(std::ostream& o, const Vector2& v){
		o << "Vector2(" << v.x << "," << v.y << ")" << std::endl;
		return o;
	}

	inline Vector2  operator-(const Vector2  &a) const{
		return Vector2(x - a.x,y - a.y);
	}

	inline Vector2  operator+(const Vector2  &a) const{
		return Vector2(x + a.x,y + a.y);
	}
};

