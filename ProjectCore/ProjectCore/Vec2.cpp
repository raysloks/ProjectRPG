#include "Vec2.h"
#include "Vec3.h"

Vec2::Vec2(void) : x(0.0f), y(0.0f)
{
}

Vec2::Vec2(float a, float b) : x(a), y(b)
{
}

Vec2::Vec2(instream& is)
{
	is >> x >> y;
}

Vec2::~Vec2(void)
{
}

Vec2::operator Vec3(void) const
{
	return Vec3(x, y, 0.0f);
}

float Vec2::Len(void)const {
	return sqrt(x*x + y*y);
}

float Vec2::LenPwr(void)const {
	return x*x + y*y;
}

void Vec2::Normalize(void) {
	float l = LenPwr();
	if (l!=0.0f)
		*this /= sqrt(l);
}

float Vec2::Dot(const Vec2& rhs)const {
	return x*rhs.x + y*rhs.y;
}

void Vec2::Truncate(float lim) {
	float l = LenPwr();
	if (l==0.0f) {
		x = 0.0f;
		y = 0.0f;
		return;
	}
	if (l>lim*lim) {
		*this = *this/sqrt(l)*lim;
	}
}

const Vec2& Vec2::operator+=(const Vec2& rhs) {
	x+=rhs.x;
	y+=rhs.y;
	return *this;
}

const Vec2& Vec2::operator-=(const Vec2& rhs) {
	x-=rhs.x;
	y-=rhs.y;
	return *this;
}

const Vec2& Vec2::operator*=(const Vec2& rhs) {
	x*=rhs.x;
	y*=rhs.y;
	return *this;
}

const Vec2& Vec2::operator*=(float rhs) {
	x*=rhs;
	y*=rhs;
	return *this;
}

const Vec2& Vec2::operator/=(float rhs) {
	x/=rhs;
	y/=rhs;
	return *this;
}

Vec2 Vec2::operator+(const Vec2& rhs)const {
	return Vec2(x+rhs.x, y+rhs.y);
}

Vec2 Vec2::operator-(const Vec2& rhs)const {
	return Vec2(x-rhs.x, y-rhs.y);
}

Vec2 Vec2::operator*(const Vec2& rhs)const {
	return Vec2(x*rhs.x, y*rhs.y);
}

Vec2 Vec2::operator*(float rhs)const {
	return Vec2(x*rhs, y*rhs);
}

Vec2 Vec2::operator/(float rhs)const {
	return Vec2(x/rhs, y/rhs);
}

bool Vec2::operator!=(const Vec2& rhs)const {
	return x!=rhs.x || y!=rhs.y;
}

bool Vec2::operator==(const Vec2& rhs)const {
	return x==rhs.x && y==rhs.y;
}

bool Vec2::operator<(const Vec2& rhs)const {
	return x<rhs.x && y<rhs.y;
}

bool Vec2::operator<=(const Vec2& rhs)const {
	return x<=rhs.x && y<=rhs.y;
}

bool Vec2::operator>(const Vec2& rhs)const {
	return x>rhs.x && y>rhs.y;
}

bool Vec2::operator>=(const Vec2& rhs)const {
	return x>=rhs.x && y>=rhs.y;
}

Vec2 operator-(const Vec2& rhs) {
	return Vec2(-rhs.x, -rhs.y);
}

outstream& operator<<(outstream& os, const Vec2& vec)
{
	os << vec.x << vec.y;
	return os;
}

instream& operator>>(instream& is, Vec2& vec)
{
	is >> vec.x >> vec.y;
	return is;
}