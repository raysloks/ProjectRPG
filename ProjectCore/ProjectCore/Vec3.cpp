#include "Vec3.h"
#include "Vec2.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "Quaternion.h"

Vec3::Vec3(void) : x(0.0f), y(0.0f), z(0.0f)
{
}

Vec3::Vec3(float a, float b, float c) : x(a), y(b), z(c)
{
}

Vec3::Vec3(instream& is)
{
	is >> x >> y >> z;
}

Vec3::~Vec3(void)
{
}

Vec3::operator Vec2(void)
{
	return Vec2(x, y);
}

float Vec3::Len(void)const {
	return sqrt(x*x + y*y + z*z);
}

float Vec3::LenPwr(void)const {
	return x*x + y*y + z*z;
}

Vec3& Vec3::Normalize(void) {
	float l = Len();
	if (l != 0.0f)
		*this /= l;
	return *this;
}

Vec3 Vec3::Normalized(void) {
	float l = Len();
	if (l != 0.0f)
		return *this / l;
	return *this;
}

float Vec3::Dot(const Vec3& rhs)const {
	return x*rhs.x + y*rhs.y + z*rhs.z;
}

Vec3 Vec3::Cross(const Vec3& rhs)const {
	return Vec3(y*rhs.z-z*rhs.y, z*rhs.x-x*rhs.z, x*rhs.y-y*rhs.x);
}

const Vec3& Vec3::operator+=(const Vec3& rhs) {
	x+=rhs.x;
	y+=rhs.y;
	z+=rhs.z;
	return *this;
}

const Vec3& Vec3::operator-=(const Vec3& rhs) {
	x-=rhs.x;
	y-=rhs.y;
	z-=rhs.z;
	return *this;
}

const Vec3& Vec3::operator*=(const Vec3& rhs) {
	x*=rhs.x;
	y*=rhs.y;
	z*=rhs.z;
	return *this;
}

const Vec3& Vec3::operator*=(float rhs) {
	x*=rhs;
	y*=rhs;
	z*=rhs;
	return *this;
}

const Vec3& Vec3::operator/=(float rhs) {
	x/=rhs;
	y/=rhs;
	z/=rhs;
	return *this;
}

Vec3 Vec3::operator+(const Vec3& rhs)const {
	return Vec3(x+rhs.x, y+rhs.y, z+rhs.z);
}

Vec3 Vec3::operator-(const Vec3& rhs)const {
	return Vec3(x-rhs.x, y-rhs.y, z-rhs.z);
}

Vec3 Vec3::operator*(const Vec3& rhs)const {
	return Vec3(x*rhs.x, y*rhs.y, z*rhs.z);
}

Vec3 Vec3::operator*(float rhs)const {
	return Vec3(x*rhs, y*rhs, z*rhs);
}

Vec3 Vec3::operator/(float rhs)const {
	return Vec3(x/rhs, y/rhs, z/rhs);
}

bool Vec3::operator!=(const Vec3& rhs)const {
	return x!=rhs.x || y!=rhs.y || z!=rhs.z;
}

bool Vec3::operator==(const Vec3& rhs)const {
	return x==rhs.x && y==rhs.y && z==rhs.z;
}

bool Vec3::operator<(const Vec3& rhs)const {
	return x<rhs.x && y<rhs.y && z<rhs.z;
}

bool Vec3::operator<=(const Vec3& rhs)const {
	return x<=rhs.x && y<=rhs.y && z<=rhs.z;
}

bool Vec3::operator>(const Vec3& rhs)const {
	return x>rhs.x && y>rhs.y && z>rhs.z;
}

bool Vec3::operator>=(const Vec3& rhs)const {
	return x>=rhs.x && y>=rhs.y && z>=rhs.z;
}

Vec3 Vec3::operator*(const Matrix3& rhs)const {
	return Vec3(x * rhs.data[0] + y * rhs.data[3] + z * rhs.data[6],
		x * rhs.data[1] + y * rhs.data[4] + z * rhs.data[7],
		x * rhs.data[2] + y * rhs.data[5] + z * rhs.data[8]);
}

Vec3 Vec3::operator*(const Matrix4& rhs)const {
	float w = x * rhs.data[3] + y * rhs.data[7] + z * rhs.data[11] + rhs.data[15];
	return Vec3(x * rhs.data[0] + y * rhs.data[4] + z * rhs.data[8] + rhs.data[12],
		x * rhs.data[1] + y * rhs.data[5] + z * rhs.data[9] + rhs.data[13],
		x * rhs.data[2] + y * rhs.data[6] + z * rhs.data[10] + rhs.data[14])/w;
}

Vec3 Vec3::operator*(const Quaternion& q) const
{
	float x2 = q.x * q.x;
	float y2 = q.y * q.y;
	float z2 = q.z * q.z;
	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float yz = q.y * q.z;
	float wx = q.w * q.x;
	float wy = q.w * q.y;
	float wz = q.w * q.z;

	Vec3 ret;

	float m0 = 1.0f - 2.0f * (y2 + z2);
	float m4 = 2.0f * (xy - wz);
	float m8 = 2.0f * (xz + wy);
	float m1 = 2.0f * (xy + wz);
	float m5 = 1.0f - 2.0f * (x2 + z2);
	float m9 = 2.0f * (yz - wx);
	float m2 = 2.0f * (xz - wy);
	float m6 = 2.0f * (yz + wx);
	float m10 = 1.0f - 2.0f * (x2 + y2);

	ret.x = x * m0 + y * m4 + z * m8;
	ret.y = x * m1 + y * m5 + z * m9;
	ret.z = x * m2 + y * m6 + z * m10;

	return ret;
}

const Vec3& Vec3::operator*=(const Matrix3& rhs) {
	*this = Vec3(x * rhs.data[0] + y * rhs.data[3] + z * rhs.data[6],
		x * rhs.data[1] + y * rhs.data[4] + z * rhs.data[7],
		x * rhs.data[2] + y * rhs.data[5] + z * rhs.data[8]);
	return *this;
}

const Vec3& Vec3::operator*=(const Matrix4& rhs) {
	float w = x * rhs.data[3] + y * rhs.data[7] + z * rhs.data[11] + rhs.data[15];
	*this = Vec3(x * rhs.data[0] + y * rhs.data[4] + z * rhs.data[8] + rhs.data[12],
	x * rhs.data[1] + y * rhs.data[5] + z * rhs.data[9] + rhs.data[13],
	x * rhs.data[2] + y * rhs.data[6] + z * rhs.data[10] + rhs.data[14]);
	*this /= w;
	return *this;
}

const Vec3& Vec3::operator*=(const Quaternion& q)
{
	float x2 = q.x * q.x;
	float y2 = q.y * q.y;
	float z2 = q.z * q.z;
	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float yz = q.y * q.z;
	float wx = q.w * q.x;
	float wy = q.w * q.y;
	float wz = q.w * q.z;

	Vec3 ret;

	float m0 = 1.0f - 2.0f * (y2 + z2);
	float m4 = 2.0f * (xy - wz);
	float m8 = 2.0f * (xz + wy);
	float m1 = 2.0f * (xy + wz);
	float m5 = 1.0f - 2.0f * (x2 + z2);
	float m9 = 2.0f * (yz - wx);
	float m2 = 2.0f * (xz - wy);
	float m6 = 2.0f * (yz + wx);
	float m10 = 1.0f - 2.0f * (x2 + y2);

	ret.x = x * m0 + y * m4 + z * m8;
	ret.y = x * m1 + y * m5 + z * m9;
	ret.z = x * m2 + y * m6 + z * m10;

	*this = ret;

	return *this;
}

const Vec3& Vec3::operator/=(const Matrix4& rhs) {
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

Vec3 operator-(const Vec3& rhs) {
	return Vec3(-rhs.x, -rhs.y, -rhs.z);
}

outstream& operator<<(outstream& os, const Vec3& vec)
{
	os << vec.x << vec.y << vec.z;
	return os;
}

instream& operator>>(instream& is, Vec3& vec)
{
	is >> vec.x >> vec.y >> vec.z;
	return is;
}