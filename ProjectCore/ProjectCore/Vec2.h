#include "streams.h"

#ifndef VEC2_H
#define VEC2_H

class Vec3;

class Vec2
{
public:
	float x, y;

	Vec2(void);
	Vec2(float a, float b);
	Vec2(instream& is);
	~Vec2(void);

	operator Vec3(void);
	
	float Len(void)const;
	float LenPwr(void)const;

	void Normalize(void);

	float Dot(const Vec2& rhs)const;

	void Truncate(float lim);
	
	const Vec2& operator+=(const Vec2& rhs);
	const Vec2& operator-=(const Vec2& rhs);
	const Vec2& operator*=(const Vec2& rhs);
	const Vec2& operator*=(float rhs);
	const Vec2& operator/=(float rhs);
	Vec2 operator+(const Vec2& rhs)const;
	Vec2 operator-(const Vec2& rhs)const;
	Vec2 operator*(const Vec2& rhs)const;
	Vec2 operator*(float rhs)const;
	Vec2 operator/(float rhs)const;

	bool operator!=(const Vec2& rhs)const;
	bool operator==(const Vec2& rhs)const;
	bool operator<(const Vec2& rhs)const;
	bool operator<=(const Vec2& rhs)const;
	bool operator>(const Vec2& rhs)const;
	bool operator>=(const Vec2& rhs)const;
};

Vec2 operator-(const Vec2& rhs);

outstream& operator<<(outstream& os, const Vec2& vec);
instream& operator>>(instream& is, Vec2& vec);

#endif