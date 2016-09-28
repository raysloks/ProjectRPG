#include "streams.h"
#include "Vec3.h"

#ifndef MATRIX3_H
#define MATRIX3_H

class Matrix4;

class Matrix3
{
public:
	union {
		float data[9];
		float mtrx[3][3];
	};

	Matrix3(void);
	Matrix3(float angle, const Vec3& axis);
	Matrix3(const Vec3& x, const Vec3& y, const Vec3& z);
	Matrix3(const Matrix4& mtrx);
	~Matrix3(void);

	float Determinant(void)const;

	Matrix3 Inverse(void)const;
	Matrix3 Transpose(void)const;

	const Matrix3& operator*=(float rhs);
	const Matrix3& operator/=(float rhs);

	Matrix3 operator+(const Matrix3& rhs)const;

	Matrix3 operator*(float rhs)const;
	Matrix3 operator*(const Matrix3& rhs)const;
	Vec3 operator*(const Vec3& rhs)const;

	bool operator==(const Matrix3& rhs)const;
	bool operator!=(const Matrix3& rhs)const;
};

outstream& operator<<(outstream& os, const Matrix3& mtrx);
instream& operator>>(instream& is, Matrix3& mtrx);

#endif