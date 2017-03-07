#include "Vec3.h"

#ifndef QUATERNION_H
#define QUATERNION_H

class Quaternion
{
public:
	float w, x, y, z;

	Quaternion(void);
	Quaternion(float a, float b, float c, float d) : w(a), x(b), y(c), z(d){}
	Quaternion(const Vec3& vRot);
	Quaternion(float dAngle, const Vec3& vAxis);
	~Quaternion(void);

	Quaternion operator*(const Quaternion& rhs)const;
	const Quaternion& operator*=(const Quaternion& rhs);

	void Normalize(void);

	Quaternion getConj(void)const;

	static Quaternion lookAt(const Vec3& forward, const Vec3& up);
};

#endif