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

Quaternion Quaternion::getConj(void) const
{
	return Quaternion(w, -x, -y, -z);
}

Quaternion Quaternion::lookAt(const Vec3& forward, const Vec3& up)
{
	/*Vec3 right = forward.Cross(up);

	Quaternion ret;
	float a = 1.0f + right.x + up.y + forward.z;
	ret.w = sqrt(a) * 0.5f;
	float c = 1.0f / (4.0f * ret.w);
	ret.x = (forward.y - up.z) * c;
	ret.y = (right.z - forward.x) * c;
	ret.z = (up.x - right.y) * c;

	return ret;*/

	/*Quaternion ret;

	Vec3 x(up.Cross(forward));
	Vec3 y(x.Cross(up));
	Vec3 z(y.Cross(x));

	float tr = x.x + y.y + z.z;

	ret.x = y.z - z.y;
	ret.y = z.x - x.z;
	ret.z = x.y - y.x;
	ret.w = tr + 1.0f;

	ret.Normalize();

	return ret;*/

	Quaternion ret;

	Vec3 z(forward);
	z.Normalize();
	Vec3 x(up.Cross(z));
	x.Normalize();
	Vec3 y(z.Cross(x));
	ret.w = sqrt(1.0f + x.x + y.y + z.z) / 2.0f;
	float w4 = ret.w * 4.0f;
	if (w4 == 0.0f)
	{
		ret.x = z.y - y.z;
		ret.y = x.z - z.x;
		ret.z = y.x - x.y;
	}
	else
	{
		ret.x = (z.y - y.z) / w4;
		ret.y = (x.z - z.x) / w4;
		ret.z = (y.x - x.y) / w4;
	}

	ret.Normalize();

	return ret;
}

outstream& operator<<(outstream& os, const Quaternion& q)
{
	os << q.w << q.x << q.y << q.z;
	return os;
}

instream& operator >> (instream& is, Quaternion& q)
{
	is >> q.w >> q.x >> q.y >> q.z;
	return is;
}