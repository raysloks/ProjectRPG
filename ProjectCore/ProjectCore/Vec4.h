#include "streams.h"

#ifndef VEC4_H
#define VEC4_H

class Vec2;
class Vec3;
class Matrix3;
class Matrix4;

class Vec4
{
public:
	float x, y, z, w;

	Vec4(void);
	Vec4(float a);
	Vec4(float a, float b, float c, float d);
	Vec4(const Vec3& a, float b);
	Vec4(instream& is);
	~Vec4(void);

	operator Vec2(void);
	operator Vec3(void);
	
	float Len(void)const;
	float LenPwr(void)const;

	void Normalize(void);

	float Dot(const Vec4& rhs)const;

	Vec4 Cross(const Vec4& rhs)const;
	
	const Vec4& operator+=(const Vec4& rhs);
	const Vec4& operator-=(const Vec4& rhs);
	const Vec4& operator*=(const Vec4& rhs);
	const Vec4& operator*=(float rhs);
	const Vec4& operator/=(float rhs);
	Vec4 operator+(const Vec4& rhs)const;
	Vec4 operator-(const Vec4& rhs)const;
	Vec4 operator*(const Vec4& rhs)const;
	Vec4 operator*(float rhs)const;
	Vec4 operator/(float rhs)const;

	bool operator!=(const Vec4& rhs)const;
	bool operator==(const Vec4& rhs)const;
	bool operator<(const Vec4& rhs)const;
	bool operator<=(const Vec4& rhs)const;
	bool operator>(const Vec4& rhs)const;
	bool operator>=(const Vec4& rhs)const;
	
	Vec4 operator*(const Matrix3& rhs)const;
	Vec4 operator*(const Matrix4& rhs)const;
	
	const Vec4& operator*=(const Matrix3& rhs);
	const Vec4& operator*=(const Matrix4& rhs);

	const Vec4& operator/=(const Matrix4& rhs);
};

Vec4 operator-(const Vec4& rhs);

#endif