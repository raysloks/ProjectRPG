#include "Vec4.h"
#include "Vec3.h"
#include "Vec2.h"
#include "Matrix3.h"
#include "Matrix4.h"

Vec4::Vec4(void) : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
}

Vec4::Vec4(float a, float b, float c, float d) : x(a), y(b), z(c), w(d)
{
}

Vec4::Vec4(instream& is)
{
	is >> x >> y >> z >> w;
}

Vec4::~Vec4(void)
{
}

Vec4::operator Vec2(void)
{
	return Vec2(x, y);
}

Vec4::operator Vec3(void)
{
	return Vec3(x, y, z);
}

float Vec4::Len(void)const {
	return sqrt(x*x + y*y + z*z + w*w);
}

float Vec4::LenPwr(void)const {
	return x*x + y*y + z*z + w*w;
}

void Vec4::Normalize(void) {
	float l = LenPwr();
	if (l!=0.0f)
		*this /= sqrt(l);
}

float Vec4::Dot(const Vec4& rhs)const {
	return x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w;
}

Vec4 Vec4::Cross(const Vec4& rhs)const {
	return Vec4(y*rhs.z-w*rhs.z, z*rhs.w-x*rhs.w, x*rhs.y-y*rhs.w, x*rhs.y-z*rhs.y);
}

const Vec4& Vec4::operator+=(const Vec4& rhs) {
	x+=rhs.x;
	y+=rhs.y;
	z+=rhs.z;
	w+=rhs.w;
	return *this;
}

const Vec4& Vec4::operator-=(const Vec4& rhs) {
	x-=rhs.x;
	y-=rhs.y;
	z-=rhs.z;
	w-=rhs.w;
	return *this;
}

const Vec4& Vec4::operator*=(const Vec4& rhs) {
	x*=rhs.x;
	y*=rhs.y;
	z*=rhs.z;
	w*=rhs.w;
	return *this;
}

const Vec4& Vec4::operator*=(float rhs) {
	x*=rhs;
	y*=rhs;
	z*=rhs;
	w*=rhs;
	return *this;
}

const Vec4& Vec4::operator/=(float rhs) {
	x/=rhs;
	y/=rhs;
	z/=rhs;
	w/=rhs;
	return *this;
}

Vec4 Vec4::operator+(const Vec4& rhs)const {
	return Vec4(x+rhs.x, y+rhs.y, z+rhs.z, w+rhs.w);
}

Vec4 Vec4::operator-(const Vec4& rhs)const {
	return Vec4(x-rhs.x, y-rhs.y, z-rhs.z, w-rhs.w);
}

Vec4 Vec4::operator*(const Vec4& rhs)const {
	return Vec4(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w);
}

Vec4 Vec4::operator*(float rhs)const {
	return Vec4(x*rhs, y*rhs, z*rhs, w*rhs);
}

Vec4 Vec4::operator/(float rhs)const {
	return Vec4(x/rhs, y/rhs, z/rhs, w/rhs);
}

bool Vec4::operator!=(const Vec4& rhs)const {
	return x!=rhs.x || y!=rhs.y || z!=rhs.z || w!=rhs.w;
}

bool Vec4::operator==(const Vec4& rhs)const {
	return x==rhs.x && y==rhs.y && z==rhs.z && w==rhs.w;
}

bool Vec4::operator<(const Vec4& rhs)const {
	return x<rhs.x && y<rhs.y && z<rhs.z && w<rhs.w;
}

bool Vec4::operator<=(const Vec4& rhs)const {
	return x<=rhs.x && y<=rhs.y && z<=rhs.z && w<=rhs.w;
}

bool Vec4::operator>(const Vec4& rhs)const {
	return x>rhs.x && y>rhs.y && z>rhs.z && w>rhs.w;
}

bool Vec4::operator>=(const Vec4& rhs)const {
	return x>=rhs.x && y>=rhs.y && z>=rhs.z && w>=rhs.w;
}

Vec4 Vec4::operator*(const Matrix3& rhs)const {
	return Vec4(x * rhs.data[0] + y * rhs.data[3] + z * rhs.data[6],
		x * rhs.data[1] + y * rhs.data[4] + z * rhs.data[7],
		x * rhs.data[2] + y * rhs.data[5] + z * rhs.data[8],
		w);
}

Vec4 Vec4::operator*(const Matrix4& rhs)const {
	return Vec4(x * rhs.data[0] + y * rhs.data[4] + z * rhs.data[8] + w * rhs.data[12],
		x * rhs.data[1] + y * rhs.data[5] + z * rhs.data[9] + w * rhs.data[13],
		x * rhs.data[2] + y * rhs.data[6] + z * rhs.data[10] + w * rhs.data[14],
		x * rhs.data[3] + y * rhs.data[7] + z * rhs.data[11] + w * rhs.data[15]);
}

const Vec4& Vec4::operator*=(const Matrix3& rhs) {
	*this = Vec4(x * rhs.data[0] + y * rhs.data[3] + z * rhs.data[6],
		x * rhs.data[1] + y * rhs.data[4] + z * rhs.data[7],
		x * rhs.data[2] + y * rhs.data[5] + z * rhs.data[8],
		w);
	return *this;
}

const Vec4& Vec4::operator*=(const Matrix4& rhs) {
	*this = Vec4(x * rhs.data[0] + y * rhs.data[4] + z * rhs.data[8] + w * rhs.data[12],
		x * rhs.data[1] + y * rhs.data[5] + z * rhs.data[9] + w * rhs.data[13],
		x * rhs.data[2] + y * rhs.data[6] + z * rhs.data[10] + w * rhs.data[14],
		x * rhs.data[3] + y * rhs.data[7] + z * rhs.data[11] + w * rhs.data[15]);
	return *this;
}

const Vec4& Vec4::operator/=(const Matrix4& rhs) {
	Matrix3 lin = Matrix3();
	lin.data[0] = rhs.data[0];
	lin.data[1] = rhs.data[1];
	lin.data[2] = rhs.data[2];
	lin.data[3] = rhs.data[4];
	lin.data[4] = rhs.data[5];
	lin.data[5] = rhs.data[6];
	lin.data[6] = rhs.data[8];
	lin.data[7] = rhs.data[9];
	lin.data[8] = rhs.data[10];
	lin = lin.Inverse();
	x -= rhs.data[12];
	y -= rhs.data[13];
	z -= rhs.data[14];
	*this *= lin;
	return *this;
}

Vec4 operator-(const Vec4& rhs) {
	return Vec4(-rhs.x, -rhs.y, -rhs.z, -rhs.w);
}

outstream& operator<<(outstream& os, const Vec4& vec)
{
	os << vec.x << vec.y << vec.z << vec.w;
	return os;
}

instream& operator>>(instream& is, Vec4& vec)
{
	is >> vec.x >> vec.y >> vec.z >> vec.w;
	return is;
}