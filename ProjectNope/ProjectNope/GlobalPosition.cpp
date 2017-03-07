#include "GlobalPosition.h"

GlobalPosition::GlobalPosition(void)
{
	local_pos = Vec3();
	x = 0;
	y = 0;
	z = 0;
}

GlobalPosition::GlobalPosition(const Vec3& pos)
{
	local_pos = pos;
	x = 0;
	y = 0;
	z = 0;
	_adjust();
}

GlobalPosition::GlobalPosition(const Vec3& local, int px, int py, int pz)
{
	local_pos = local;
	x = px;
	y = py;
	z = pz;
	_adjust();
}

GlobalPosition::~GlobalPosition(void)
{
}

const GlobalPosition& GlobalPosition::operator+=(const GlobalPosition& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	local_pos+=rhs.local_pos;
	_adjust();
	return *this;
}

const GlobalPosition& GlobalPosition::operator-=(const GlobalPosition& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	local_pos-=rhs.local_pos;
	_adjust();
	return *this;
}

GlobalPosition GlobalPosition::operator+(const GlobalPosition& rhs)const
{
	GlobalPosition ret = rhs;
	ret.x += x;
	ret.y += y;
	ret.z += z;
	ret += local_pos;
	return ret;
}

GlobalPosition GlobalPosition::operator-(const GlobalPosition& rhs)const
{
	GlobalPosition ret = *this;
	ret.x -= rhs.x;
	ret.y -= rhs.y;
	ret.z -= rhs.z;
	ret -= rhs.local_pos;
	return ret;
}

const GlobalPosition& GlobalPosition::operator+=(const Vec3& rhs)
{
	local_pos+=rhs;
	_adjust();
	return *this;
}

const GlobalPosition& GlobalPosition::operator-=(const Vec3& rhs)
{
	local_pos-=rhs;
	_adjust();
	return *this;
}

GlobalPosition GlobalPosition::operator+(const Vec3& rhs)const
{
	GlobalPosition ret = *this;
	ret += rhs;
	return ret;
}

GlobalPosition GlobalPosition::operator-(const Vec3& rhs)const
{
	GlobalPosition ret = *this;
	ret -= rhs;
	return ret;
}

GlobalPosition GlobalPosition::extend(const Vec3& v, float l)
{
	GlobalPosition ret;
	size_t i = 0;
	for (i = 0; i < l; i++)
	{
		ret += v;
		ret._adjust();
	}
	ret += v * (l - (i - 1));
	ret._adjust();
	return ret;
}

//const GlobalPosition& GlobalPosition::operator*=(float rhs)
//{
//	local_pos*=rhs;
//	float x2 = ((float)x)*rhs;
//	float y2 = ((float)y)*rhs;
//	float z2 = ((float)z)*rhs;
//	x *= rhs;
//	y *= rhs;
//	z *= rhs;
//	local_pos+=Vec3(x2-x, y2-y, z2-z);
//	_adjust();
//	return *this;
//}
//
//const GlobalPosition& GlobalPosition::operator/=(float rhs)
//{
//	local_pos/=rhs;
//	float x2 = ((float)x)/rhs;
//	float y2 = ((float)y)/rhs;
//	float z2 = ((float)z)/rhs;
//	x /= rhs;
//	y /= rhs;
//	z /= rhs;
//	local_pos+=Vec3(x2-x, y2-y, z2-z);
//	_adjust();
//	return *this;
//}
//
//GlobalPosition GlobalPosition::operator*(float rhs)const
//{
//	GlobalPosition ret(*this);
//	ret.local_pos*=rhs;
//	float x2 = ((float)x)*rhs;
//	float y2 = ((float)y)*rhs;
//	float z2 = ((float)z)*rhs;
//	ret.x = x*rhs;
//	ret.y = y*rhs;
//	ret.z = z*rhs;
//	ret.local_pos+=Vec3(x2-ret.x, y2-ret.y, z2-ret.z);
//	ret._adjust();
//	return ret;
//}
//
//GlobalPosition GlobalPosition::operator/(float rhs)const
//{
//	GlobalPosition ret(*this);
//	ret.local_pos/=rhs;
//	float x2 = ((float)x)/rhs;
//	float y2 = ((float)y)/rhs;
//	float z2 = ((float)z)/rhs;
//	ret.x = x/rhs;
//	ret.y = y/rhs;
//	ret.z = z/rhs;
//	ret.local_pos+=Vec3(x2-ret.x, y2-ret.y, z2-ret.z);
//	ret._adjust();
//	return ret;
//}

void GlobalPosition::_adjust(void)
{
	int offset;
	if (abs(local_pos.x)>GP_SCALE)
	{
		offset = local_pos.x/GP_SCALE;
		local_pos.x -= offset*GP_SCALE;
		x += offset;
	}
	if (abs(local_pos.y)>GP_SCALE)
	{
		offset = local_pos.y/GP_SCALE;
		local_pos.y -= offset*GP_SCALE;
		y += offset;
	}
	if (abs(local_pos.z)>GP_SCALE)
	{
		offset = local_pos.z/GP_SCALE;
		local_pos.z -= offset*GP_SCALE;
		z += offset;
	}
}

GlobalPosition::operator Vec3(void)
{
	Vec3 v = local_pos;
	v.x += x*GP_SCALE;
	v.y += y*GP_SCALE;
	v.z += z*GP_SCALE;
	return v;
}

GlobalPosition::operator Vec3(void) const
{
	Vec3 v = local_pos;
	v.x += x*GP_SCALE;
	v.y += y*GP_SCALE;
	v.z += z*GP_SCALE;
	return v;
}

bool GlobalPosition::operator !=(const GlobalPosition& rhs)
{
	return local_pos != rhs.local_pos || x != rhs.x || y != rhs.y || z != rhs.z;
}

bool GlobalPosition::operator ==(const GlobalPosition& rhs)
{
	return local_pos == rhs.local_pos && x == rhs.x && y == rhs.y && z == rhs.z;
}

outstream& operator<<(outstream& os, const GlobalPosition& pos)
{
	os << pos.local_pos << pos.x << pos.y << pos.z;
	return os;
}

instream& operator>>(instream& is, GlobalPosition& pos)
{
	is >> pos.local_pos >> pos.x >> pos.y >> pos.z;
	return is;
}