#include "streams.h"

#ifndef MATRIX4_H
#define MATRIX4_H

class Vec3;
class Matrix3;
class Quaternion;

class Matrix4
{
public:
	union {
		float data[16];
		float mtrx[4][4];
	};

	Matrix4(void);
	Matrix4(const Matrix3& mtrx);
	Matrix4(const Quaternion& q);
	~Matrix4(void);

	float Determinant(void)const;
	Matrix4 Inverse(void)const;
	Matrix4 Transpose(void)const;
	
	Matrix4 operator+(const Matrix4& rhs)const;
	Matrix4 operator*(const Matrix4& rhs)const;
	const Matrix4& operator+=(const Matrix4& rhs);
	const Matrix4& operator*=(const Matrix4& rhs);

	Matrix4 operator*(float rhs)const;
	Matrix4 operator/(float rhs)const;
	const Matrix4& operator*=(float rhs);
	const Matrix4& operator/=(float rhs);

	bool operator!=(const Matrix4& rhs)const;
	bool operator==(const Matrix4& rhs)const;

	static Matrix4 Translation(const Vec3& v);
	static Matrix4 Perspective(float fov, float aspect, float near, float far);
};

outstream& operator<<(outstream& os, const Matrix4& m);
instream& operator>>(instream& is, Matrix4& m);

#endif