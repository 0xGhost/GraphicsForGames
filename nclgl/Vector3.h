#pragma once
/*
Class:Vector3
Implements:
Author:Rich Davison
Description:VERY simple Vector3 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
#include <cmath>
#include <iostream>

class Vector3	{
public:
	Vector3(void) {
		ToZero();
	}

	Vector3(const float x, const float y, const float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	~Vector3(void){}

	float x;
	float y;
	float z;

	void			Normalise() {
		float length = Length();

		if(length != 0.0f)	{
			length = 1.0f / length;
			x = x * length;
			y = y * length;
			z = z * length;
		}
	}

	void		ToZero() {
		x = y = z = 0.0f;
	}

	float			Length() const {
		return sqrt((x*x)+(y*y)+(z*z));	
	}

	void			Invert() {
		x = -x;
		y = -y;	
		z = -z;	
	}

	Vector3			Inverse() const{
		return Vector3(-x,-y,-z);
	}

	static float	Dot(const Vector3 &a, const Vector3 &b) {
		return (a.x*b.x)+(a.y*b.y)+(a.z*b.z);
	}

	static Vector3	Cross(const Vector3 &a, const Vector3 &b) {
		return Vector3((a.y*b.z) - (a.z*b.y) , (a.z*b.x) - (a.x*b.z) , (a.x*b.y) - (a.y*b.x));	
	}

	inline friend std::ostream& operator<<(std::ostream& o, const Vector3& v) {
		o << "Vector3(" << v.x << "," << v.y << "," << v.z <<")" << std::endl;
		return o;
	}

	inline Vector3  operator+(const Vector3  &a) const{
		return Vector3(x + a.x,y + a.y, z + a.z);
	}

	inline Vector3  operator-(const Vector3  &a) const{
		return Vector3(x - a.x,y - a.y, z - a.z);
	}

	inline Vector3  operator-() const{
		return Vector3(-x,-y,-z);
	}

	inline void operator+=(const Vector3  &a){
		x += a.x;
		y += a.y;
		z += a.z;
	}

	inline void operator-=(const Vector3  &a){
		x -= a.x;
		y -= a.y;
		z -= a.z;
	}

	inline Vector3  operator*(const float a) const{
		return Vector3(x * a,y * a, z * a);
	}

	inline Vector3  operator*(const Vector3  &a) const{
		return Vector3(x * a.x,y * a.y, z * a.z);
	}

	inline Vector3  operator/(const Vector3  &a) const{
		return Vector3(x / a.x,y / a.y, z / a.z);
	};

	inline Vector3  operator/(const float v) const{
		return Vector3(x / v,y / v, z / v);
	};

	inline bool	operator==(const Vector3 &A)const {return (A.x == x && A.y == y && A.z == z) ? true : false;};
	inline bool	operator!=(const Vector3 &A)const {return (A.x == x && A.y == y && A.z == z) ? false : true;};
};

