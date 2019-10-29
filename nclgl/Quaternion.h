/******************************************************************************
Class:Quaternion
Implements:
Author:Rich Davison
Description:VERY simple Quaternion class. Students are encouraged to modify 
this as necessary...if they dare.

Quaternions aren't really discussed much in the graphics module, but the 
MD5Mesh class uses them internally, as MD5 files store their rotations
as quaternions.

I hate Quaternions.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <iostream>
#include "common.h"
#include "Matrix4.h"

class Matrix4;

class Quaternion	{
public:
	Quaternion(void);
	Quaternion(float x, float y, float z, float w);

	~Quaternion(void);

	float x;
	float y;
	float z;
	float w;

	void	Normalise();
	Matrix4 ToMatrix() const;


	Quaternion	Conjugate() const;
	void		GenerateW();	//builds 4th component when loading in shortened, 3 component quaternions

	static Quaternion EulerAnglesToQuaternion(float pitch, float yaw, float roll);
	static Quaternion AxisAngleToQuaterion(const Vector3& vector, float degrees);

	static void RotatePointByQuaternion(const Quaternion &q, Vector3 &point);

	static Quaternion FromMatrix(const Matrix4 &m);

	static float Dot(const Quaternion &a, const Quaternion &b);

	Quaternion operator *(const Quaternion &a) const;
	Quaternion operator *(const Vector3 &a) const;

	inline friend std::ostream& operator<<(std::ostream& o, const Quaternion& q){
		o << "Quat(" << q.x << "," << q.y << "," << q.z <<  "," << q.w << ")" << std::endl;
		return o;
	}
};