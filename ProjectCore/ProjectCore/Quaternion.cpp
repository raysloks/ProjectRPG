#include "Quaternion.h"
#include <math.h>

Quaternion::Quaternion(void)
{
	w = 1.0;
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

Quaternion::Quaternion(const Vec3& vRot)
{
	float l = vRot.Len();
	l /= 2.0f;
	if (l>0.0f) {
		w = cos(l);
		float s = sin(l);
		x = (vRot.x/l/2.0f)*s;
		y = (vRot.y/l/2.0f)*s;
		z = (vRot.z/l/2.0f)*s;
	} else {
		w = 1.0f;
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
}

Quaternion::Quaternion(float dAngle, const Vec3& vAxis)
{
	dAngle /= 2.0f;
	w = cos(dAngle);
	float s = sin(dAngle);
	x = vAxis.x * s;
	y = vAxis.y * s;
	z = vAxis.z * s;
}

Quaternion::~Quaternion(void)
{
}

Quaternion Quaternion::operator*(const Quaternion& rhs)const
{
	return Quaternion(w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
	w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
	w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
	w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x);
}

const Quaternion& Quaternion::operator*=(const Quaternion& rhs)
{
	w = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;
	x = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
	y = w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z;
	z = w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x;
	return *this;
}

void Quaternion::Normalize(void)
{
	float l = w*w+x*x+y*y+z*z;
	if (l == 0) {
		w = 1.0f;
	} else {
		l = sqrt(l);
		w /= l;
		x /= l;
		y /= l;
		z /= l;
	}
}

Quaternion Quaternion::getConj(void)const
{
	return Quaternion(w, -x, -y, -z);
}