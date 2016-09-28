#include "streams.h"

#ifndef VEC3_H
#define VEC3_H

class Vec2;
class Matrix3;
class Matrix4;

class Vec3
{
public:
	float x, y, z;

	Vec3(void);
	Vec3(float a, float b, float c);
	Vec3(instream& is);
	~Vec3(void);

	operator Vec2(void);
	
	float Len(void)const;
	float LenPwr(void)const;
	
	Vec3& Normalize(void);

	float Dot(const Vec3& rhs)const;

	Vec3 Cross(const Vec3& rhs)const;
	
	const Vec3& operator+=(const Vec3& rhs);
	const Vec3& operator-=(const Vec3& rhs);
	const Vec3& operator*=(const Vec3& rhs);
	const Vec3& operator*=(float rhs);
	const Vec3& operator/=(float rhs);
	Vec3 operator+(const Vec3& rhs)const;
	Vec3 operator-(const Vec3& rhs)const;
	Vec3 operator*(const Vec3& rhs)const;
	Vec3 operator*(float rhs)const;
	Vec3 operator/(float rhs)const;

	bool operator!=(const Vec3& rhs)const;
	bool operator==(const Vec3& rhs)const;
	bool operator<(const Vec3& rhs)const;
	bool operator<=(const Vec3& rhs)const;
	bool operator>(const Vec3& rhs)const;
	bool operator>=(const Vec3& rhs)const;
	
	Vec3 operator*(const Matrix3& rhs)const;
	Vec3 operator*(const Matrix4& rhs)const;
	
	const Vec3& operator*=(const Matrix3& rhs);
	const Vec3& operator*=(const Matrix4& rhs);

	const Vec3& operator/=(const Matrix4& rhs);
};

Vec3 operator-(const Vec3& rhs);

outstream& operator<<(outstream& os, const Vec3& vec);
instream& operator>>(instream& is, Vec3& vec);

#endif