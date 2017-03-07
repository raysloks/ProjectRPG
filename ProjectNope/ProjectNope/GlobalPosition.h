#include "Vec3.h"
#include "streams.h"

#ifndef GLOBAL_POSITION_H
#define GLOBAL_POSITION_H

const float GP_SCALE = 10.0f;

class GlobalPosition
{
public:
	GlobalPosition(void);
	GlobalPosition(const Vec3& pos);
	GlobalPosition(const Vec3& local, int px, int py, int pz);
	~GlobalPosition(void);

	const GlobalPosition& operator+=(const GlobalPosition& rhs);
	const GlobalPosition& operator-=(const GlobalPosition& rhs);
	GlobalPosition operator+(const GlobalPosition& rhs)const;
	GlobalPosition operator-(const GlobalPosition& rhs)const;

	const GlobalPosition& operator+=(const Vec3& rhs);
	const GlobalPosition& operator-=(const Vec3& rhs);
	GlobalPosition operator+(const Vec3& rhs)const;
	GlobalPosition operator-(const Vec3& rhs)const;
	
	static GlobalPosition extend(const Vec3& v, float l);

	/*const GlobalPosition& operator*=(float rhs);
	const GlobalPosition& operator/=(float rhs);
	GlobalPosition operator*(float rhs)const;
	GlobalPosition operator/(float rhs)const;*/

	operator Vec3(void);
	operator Vec3(void) const;

	bool operator !=(const GlobalPosition& rhs);
	bool operator ==(const GlobalPosition& rhs);

	Vec3 local_pos;
	int x, y, z;

private:
	void _adjust(void);
};

outstream& operator<<(outstream& os, const GlobalPosition& pos);
instream& operator>>(instream& is, GlobalPosition& pos);

#endif