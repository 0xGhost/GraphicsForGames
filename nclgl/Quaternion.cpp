#include "Quaternion.h"

Quaternion::Quaternion(void)
{
	x = y = z = 0.0f;
	w = 1.0f;
}

Quaternion::Quaternion(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Quaternion::~Quaternion(void)
{
}

float Quaternion::Dot(const Quaternion &a,const Quaternion &b){
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

void Quaternion::Normalise(){
	float magnitude = sqrt(Dot(*this,*this));

	if(magnitude > 0.0f){
		float t= 1.0f / magnitude;

		x *= t;
		y *= t;
		z *= t;
		w *= t;
	}
}


Quaternion Quaternion::operator *(const Quaternion &b) const{
	Quaternion ans;

	ans.w = (w * b.w) - (x * b.x) - (y * b.y) - (z * b.z);
	ans.x = (x * b.w) + (w * b.x) + (y * b.z) - (z * b.y);
	ans.y = (y * b.w) + (w * b.y) + (z * b.x) - (x * b.z);
	ans.z = (z * b.w) + (w * b.z) + (x * b.y) - (y * b.x);

	return ans;
}

Quaternion Quaternion::operator *(const Vector3 &b) const{
	Quaternion ans;

	ans.w = -(x * b.x) - (y * b.y) - (z * b.z);
	ans.x =  (w * b.x) + (y * b.z) - (z * b.y);
	ans.y =  (w * b.y) + (z * b.x) - (x * b.z);
	ans.z =  (w * b.z) + (x * b.y) - (y * b.x);

	return ans;
}

Matrix4 Quaternion::ToMatrix() const{
	Matrix4 mat;

	float yy = y*y;
	float zz = z*z;
	float xy = x*y;
	float zw = z*w;
	float xz = x*z;
	float yw = y*w;
	float xx = x*x;
	float yz = y*z;
	float xw = x*w;

	mat.values[0] = 1 - 2*yy - 2*zz;
	mat.values[1] = 2*xy + 2*zw;
	mat.values[2] = 2*xz - 2*yw;	

	mat.values[4] = 2*xy - 2*zw;
	mat.values[5] = 1- 2*xx - 2*zz;
	mat.values[6] = 2*yz + 2*xw;

	mat.values[8] = 2*xz + 2*yw;
	mat.values[9] = 2*yz - 2*xw;
	mat.values[10] = 1 - 2*xx - 2*yy;

	return mat;
}

Quaternion Quaternion::EulerAnglesToQuaternion(float pitch, float yaw, float roll)	{
	float y2 = (float)DegToRad(yaw/2.0f);
	float p2 = (float)DegToRad(pitch/2.0f);
	float r2 = (float)DegToRad(roll/2.0f);


	float cosy   = (float)cos(y2);
	float cosp   = (float)cos(p2);
	float cosr   = (float)cos(r2);

	float siny   = (float)sin(y2);
	float sinp   = (float)sin(p2);
	float sinr   = (float)sin(r2);

	Quaternion q;


	q.x = cosr * sinp * cosy + sinr * cosp * siny;
	q.y = cosr * cosp * siny - sinr * sinp * cosy;
	q.z = sinr * cosp * cosy - cosr * sinp * siny;
	q.w = cosr * cosp * cosy + sinr * sinp * siny;

	return q;
};

Quaternion Quaternion::AxisAngleToQuaterion(const Vector3& vector, float degrees)	{
	float theta = (float)DegToRad(degrees);
	float result = (float)sin( theta / 2.0f );

	return Quaternion((float)(vector.x * result), (float)(vector.y * result), (float)(vector.z * result), (float)cos( theta / 2.0f ));
}

void Quaternion::GenerateW()	{
	w = 1.0f - (x*x)-(y*y)-(z*z);
	if(w < 0.0f) {
		w = 0.0f;
	}
	else{
		w = - sqrt(w);
	}
}

Quaternion Quaternion::Conjugate() const
{
	return Quaternion(-x,-y,-z,w);
}

Quaternion Quaternion::FromMatrix(const Matrix4 &m)	{
	Quaternion q;

	q.w = sqrt(max(0.0f, (1.0f + m.values[0] + m.values[5] + m.values[10]))) / 2.0f;
	q.x = sqrt(max(0.0f, (1.0f + m.values[0] - m.values[5] - m.values[10]))) / 2.0f;
	q.y = sqrt(max(0.0f, (1.0f - m.values[0] + m.values[5] - m.values[10]))) / 2.0f;
	q.z = sqrt(max(0.0f, (1.0f - m.values[0] - m.values[5] + m.values[10]))) / 2.0f;

	q.x = (float)_copysign( q.x, m.values[9] - m.values[6] );
	q.y = (float)_copysign( q.y, m.values[2] - m.values[8] );
	q.z = (float)_copysign( q.z, m.values[4] - m.values[1] );

	return q;
}