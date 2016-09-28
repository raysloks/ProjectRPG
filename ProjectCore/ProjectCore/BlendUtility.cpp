#include "BlendUtility.h"

#include <math.h>

#include "Vec3.h"
#include "Matrix3.h"

float bu_blend(float current, float target, float a, float b, float dTime)
{
	float c = current-target;
	float csign = signbit(c) ? -1.0f : 1.0f;
	c = fabsf(c);
	if (c!=0.0f && a!=0.0f)
	{
		if (a != 0.0f)
		{
			c += b / a;

			c *= exp(a*dTime);

			c -= b / a;
		}
		else
		{
			c += b * dTime;
		}

		if (c<0.0f)
			c=0.0;

		return target + c * csign;
	}
	return target;
}

Vec3 bu_blend(const Vec3& current, const Vec3& target, float a, float b, float dTime)
{
	Vec3 dif = current-target;
	float c = dif.Len();
	Vec3 csign = dif / c;
	if (c!=0.0f && dif!=Vec3() && a!=0.0f)
	{
		if (a != 0.0f)
		{
			c += b / a;

			c *= exp(a*dTime);

			c -= b / a;
		}
		else
		{
			c += b * dTime;
		}

		if (c<0.0f)
			c=0.0;

		return target + csign * c;
	}
	return target;
}


float bu_angle(float current, float target, float a, float b, float dTime)
{
	target = fmodf(target+M_PI, M_PI*2.0f)-M_PI;
	current = fmodf(current+M_PI, M_PI*2.0f)-M_PI;

	float c = current-target;
	if (c > M_PI)
		c -= M_PI*2.0f;
	if (c < -M_PI)
		c += M_PI*2.0f;
	float csign = signbit(c) ? -1.0f : 1.0f;
	c = fabsf(c);
	if (c!=0.0f)
	{
		if (a != 0.0f)
		{
			c += b / a;

			c *= exp(a*dTime);

			c -= b / a;
		}
		else
		{
			c += b * dTime;
		}

		if (c<0.0f)
			c=0.0;
		if (c>M_PI)
			c=M_PI;
	}
	return fmodf(target + c * csign + M_PI, M_PI*2.0f)-M_PI;
}

float bu_angle(float current, float target, float a)
{
	target = fmodf(target+M_PI, M_PI*2.0f)-M_PI;
	current = fmodf(current+M_PI, M_PI*2.0f)-M_PI;

	float c = current-target;
	if (c > M_PI)
		c -= M_PI*2.0f;
	if (c < -M_PI)
		c += M_PI*2.0f;
	c *= a;
	return fmodf(target + c + M_PI, M_PI*2.0f)-M_PI;
}

Vec3 bu_sphere(Vec3 current, Vec3 target, Vec3 pref, float a, float b, float dTime)
{
	float c = current.Dot(target);
	if (c>1.0f)
		return target;
	Vec3 axis = current.Cross(target);
	if (c<-1.0f) {
		axis = pref;
		c=-1.0f;
	}
	if (axis==Vec3())
		axis = pref;
	if (axis==Vec3())
		return target;

	c = acos(c);

	if (a != 0.0f)
	{
		c += b / a;

		c *= exp(a*dTime);

		c -= b / a;
	}
	else
	{
		c += b * dTime;
	}

	if (c<0.0f)
		c=0.0;
	if (c>M_PI)
		c=M_PI;

	return target * Matrix3(c, axis);
}

Vec3 bu_sphere(Vec3 current, Vec3 target, Vec3 pref, float a)
{
	float c = current.Dot(target);
	if (c>1.0f)
		return target;
	Vec3 axis = current.Cross(target);
	if (c<-1.0f) {
		axis = pref;
		c=-1.0f;
	}
	if (axis==Vec3())
		axis = pref;
	if (axis==Vec3())
		return target;
	c = acos(c);
	c *= a;
	if (c<0.0f)
		c=0.0;
	if (c>M_PI)
		c=M_PI;
	return target * Matrix3(c, axis);
}