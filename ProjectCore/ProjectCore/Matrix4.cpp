#include "Matrix4.h"
#include "Matrix3.h"
#include "Vec3.h"
#include "Quaternion.h"

#define M_PI 3.14159265358979323846

Matrix4::Matrix4(void)
{
	data[0] = 1.0f;
	data[1] = 0.0f;
	data[2] = 0.0f;
	data[3] = 0.0f;

	data[4] = 0.0f;
	data[5] = 1.0f;
	data[6] = 0.0f;
	data[7] = 0.0f;

	data[8] = 0.0f;
	data[9] = 0.0f;
	data[10] = 1.0f;
	data[11] = 0.0f;

	data[12] = 0.0f;
	data[13] = 0.0f;
	data[14] = 0.0f;
	data[15] = 1.0f;
}

Matrix4::Matrix4(const Matrix3& mtrx)
{
	data[0] = mtrx.data[0];
	data[1] = mtrx.data[1];
	data[2] = mtrx.data[2];
	data[3] = 0.0f;

	data[4] = mtrx.data[3];
	data[5] = mtrx.data[4];
	data[6] = mtrx.data[5];
	data[7] = 0.0f;

	data[8] = mtrx.data[6];
	data[9] = mtrx.data[7];
	data[10] = mtrx.data[8];
	data[11] = 0.0f;

	data[12] = 0.0f;
	data[13] = 0.0f;
	data[14] = 0.0f;
	data[15] = 1.0f;
}

Matrix4::Matrix4(const Quaternion& q)
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

	data[0] = 1.0f - 2.0f * (y2 + z2);
	data[4] = 2.0f * (xy - wz);
	data[8] = 2.0f * (xz + wy);
	data[12] = 0.0f;
	data[1] = 2.0f * (xy + wz);
	data[5] = 1.0f - 2.0f * (x2 + z2);
	data[9] = 2.0f * (yz - wx);
	data[13] = 0.0f;
	data[2] = 2.0f * (xz - wy);
	data[6] = 2.0f * (yz + wx);
	data[10] = 1.0f - 2.0f * (x2 + y2);
	data[14] = 0.0f;
	data[3] = 0.0f;
	data[7] = 0.0f;
	data[11] = 0.0f;
	data[15] = 1.0f;
}

Matrix4::~Matrix4(void)
{
}

float Matrix4::Determinant(void)const
{
	float m_4_1 = data[4] * data[1];
	float m_5_0 = data[5] * data[0];
	float m_8_1 = data[8] * data[1];
	float m_9_0 = data[9] * data[0];
	float m_9_4 = data[9] * data[4];
	float m_12_3 = data[12] * data[3];
	float m_12_7 = data[12] * data[7];
	float m_12_11 = data[12] * data[11];
	float m_13_3 = data[13] * data[3];
	float m_13_7 = data[13] * data[7];
	float m_13_11 = data[13] * data[11];
	float m_14_3 = data[14] * data[3];
	float m_14_7 = data[14] * data[7];
	float m_14_11 = data[14] * data[11];
	float m_15_2 = data[15] * data[2];
	float m_15_6 = data[15] * data[6];
	float m_15_8 = data[15] * data[8];
	float m_15_9 = data[15] * data[9];
	float m_15_10 = data[15] * data[10];

	return m_12_3 * data[9] * data[6] - m_13_3 * data[8] * data[6] -
		m_12_3 * data[5] * data[10] + m_13_3 * data[4] * data[10] +
		m_14_3 * data[8] * data[5] - m_14_3 * m_9_4 -
		m_12_7 * data[9] * data[2] + m_13_7 * data[8] * data[2] +
		m_12_7 * data[1] * data[10] - m_13_7 * data[0] * data[10] -
		m_14_7 * m_8_1 + m_14_7 * m_9_0 +
		m_12_11 * data[5] * data[2] - m_13_11 * data[4] * data[2] -
		m_12_11 * data[1] * data[6] + m_13_11 * data[0] * data[6] +
		m_14_11 * m_4_1 - m_14_11 * m_5_0 -
		m_15_2 * data[8] * data[5] + m_15_2 * m_9_4 +
		m_15_6 * m_8_1 - m_15_6 * m_9_0 -
		m_15_10 * m_4_1 + m_15_10 * m_5_0;
}

Matrix4 Matrix4::operator+(const Matrix4& rhs)const
{
	Matrix4 ret;
	ret.data[0] = data[0]+rhs.data[0];
	ret.data[1] = data[1]+rhs.data[1];
	ret.data[2] = data[2]+rhs.data[2];
	ret.data[3] = data[3]+rhs.data[3];

	ret.data[4] = data[4]+rhs.data[4];
	ret.data[5] = data[5]+rhs.data[5];
	ret.data[6] = data[6]+rhs.data[6];
	ret.data[7] = data[7]+rhs.data[7];
	
	ret.data[8] = data[8]+rhs.data[8];
	ret.data[9] = data[9]+rhs.data[9];
	ret.data[10] = data[10]+rhs.data[10];
	ret.data[11] = data[11]+rhs.data[11];
	
	ret.data[12] = data[12]+rhs.data[12];
	ret.data[13] = data[13]+rhs.data[13];
	ret.data[14] = data[14]+rhs.data[14];
	ret.data[15] = data[15]+rhs.data[15];
	return ret;
}

Matrix4 Matrix4::operator*(const Matrix4& rhs)const
{
	Matrix4 ret;
	ret.data[0] = data[0]*rhs.data[0]+data[1]*rhs.data[4]+data[2]*rhs.data[8]+data[3]*rhs.data[12];
	ret.data[1] = data[0]*rhs.data[1]+data[1]*rhs.data[5]+data[2]*rhs.data[9]+data[3]*rhs.data[13];
	ret.data[2] = data[0]*rhs.data[2]+data[1]*rhs.data[6]+data[2]*rhs.data[10]+data[3]*rhs.data[14];
	ret.data[3] = data[0]*rhs.data[3]+data[1]*rhs.data[7]+data[2]*rhs.data[11]+data[3]*rhs.data[15];

	ret.data[4] = data[4]*rhs.data[0]+data[5]*rhs.data[4]+data[6]*rhs.data[8]+data[7]*rhs.data[12];
	ret.data[5] = data[4]*rhs.data[1]+data[5]*rhs.data[5]+data[6]*rhs.data[9]+data[7]*rhs.data[13];
	ret.data[6] = data[4]*rhs.data[2]+data[5]*rhs.data[6]+data[6]*rhs.data[10]+data[7]*rhs.data[14];
	ret.data[7] = data[4]*rhs.data[3]+data[5]*rhs.data[7]+data[6]*rhs.data[11]+data[7]*rhs.data[15];
	
	ret.data[8] = data[8]*rhs.data[0]+data[9]*rhs.data[4]+data[10]*rhs.data[8]+data[11]*rhs.data[12];
	ret.data[9] = data[8]*rhs.data[1]+data[9]*rhs.data[5]+data[10]*rhs.data[9]+data[11]*rhs.data[13];
	ret.data[10] = data[8]*rhs.data[2]+data[9]*rhs.data[6]+data[10]*rhs.data[10]+data[11]*rhs.data[14];
	ret.data[11] = data[8]*rhs.data[3]+data[9]*rhs.data[7]+data[10]*rhs.data[11]+data[11]*rhs.data[15];
	
	ret.data[12] = data[12]*rhs.data[0]+data[13]*rhs.data[4]+data[14]*rhs.data[8]+data[15]*rhs.data[12];
	ret.data[13] = data[12]*rhs.data[1]+data[13]*rhs.data[5]+data[14]*rhs.data[9]+data[15]*rhs.data[13];
	ret.data[14] = data[12]*rhs.data[2]+data[13]*rhs.data[6]+data[14]*rhs.data[10]+data[15]*rhs.data[14];
	ret.data[15] = data[12]*rhs.data[3]+data[13]*rhs.data[7]+data[14]*rhs.data[11]+data[15]*rhs.data[15];
	return ret;
}

Matrix4 Matrix4::Inverse(void)const
{
	Matrix4 inv;

    inv.data[0] = data[5]  * data[10] * data[15] - 
             data[5]  * data[11] * data[14] - 
             data[9]  * data[6]  * data[15] + 
             data[9]  * data[7]  * data[14] +
             data[13] * data[6]  * data[11] - 
             data[13] * data[7]  * data[10];

    inv.data[4] = -data[4]  * data[10] * data[15] + 
              data[4]  * data[11] * data[14] + 
              data[8]  * data[6]  * data[15] - 
              data[8]  * data[7]  * data[14] - 
              data[12] * data[6]  * data[11] + 
              data[12] * data[7]  * data[10];

    inv.data[8] = data[4]  * data[9] * data[15] - 
             data[4]  * data[11] * data[13] - 
             data[8]  * data[5] * data[15] + 
             data[8]  * data[7] * data[13] + 
             data[12] * data[5] * data[11] - 
             data[12] * data[7] * data[9];

    inv.data[12] = -data[4]  * data[9] * data[14] + 
               data[4]  * data[10] * data[13] +
               data[8]  * data[5] * data[14] - 
               data[8]  * data[6] * data[13] - 
               data[12] * data[5] * data[10] + 
               data[12] * data[6] * data[9];

    inv.data[1] = -data[1]  * data[10] * data[15] + 
              data[1]  * data[11] * data[14] + 
              data[9]  * data[2] * data[15] - 
              data[9]  * data[3] * data[14] - 
              data[13] * data[2] * data[11] + 
              data[13] * data[3] * data[10];

    inv.data[5] = data[0]  * data[10] * data[15] - 
             data[0]  * data[11] * data[14] - 
             data[8]  * data[2] * data[15] + 
             data[8]  * data[3] * data[14] + 
             data[12] * data[2] * data[11] - 
             data[12] * data[3] * data[10];

    inv.data[9] = -data[0]  * data[9] * data[15] + 
              data[0]  * data[11] * data[13] + 
              data[8]  * data[1] * data[15] - 
              data[8]  * data[3] * data[13] - 
              data[12] * data[1] * data[11] + 
              data[12] * data[3] * data[9];

    inv.data[13] = data[0]  * data[9] * data[14] - 
              data[0]  * data[10] * data[13] - 
              data[8]  * data[1] * data[14] + 
              data[8]  * data[2] * data[13] + 
              data[12] * data[1] * data[10] - 
              data[12] * data[2] * data[9];

    inv.data[2] = data[1]  * data[6] * data[15] - 
             data[1]  * data[7] * data[14] - 
             data[5]  * data[2] * data[15] + 
             data[5]  * data[3] * data[14] + 
             data[13] * data[2] * data[7] - 
             data[13] * data[3] * data[6];

    inv.data[6] = -data[0]  * data[6] * data[15] + 
              data[0]  * data[7] * data[14] + 
              data[4]  * data[2] * data[15] - 
              data[4]  * data[3] * data[14] - 
              data[12] * data[2] * data[7] + 
              data[12] * data[3] * data[6];

    inv.data[10] = data[0]  * data[5] * data[15] - 
              data[0]  * data[7] * data[13] - 
              data[4]  * data[1] * data[15] + 
              data[4]  * data[3] * data[13] + 
              data[12] * data[1] * data[7] - 
              data[12] * data[3] * data[5];

    inv.data[14] = -data[0]  * data[5] * data[14] + 
               data[0]  * data[6] * data[13] + 
               data[4]  * data[1] * data[14] - 
               data[4]  * data[2] * data[13] - 
               data[12] * data[1] * data[6] + 
               data[12] * data[2] * data[5];

    inv.data[3] = -data[1] * data[6] * data[11] + 
              data[1] * data[7] * data[10] + 
              data[5] * data[2] * data[11] - 
              data[5] * data[3] * data[10] - 
              data[9] * data[2] * data[7] + 
              data[9] * data[3] * data[6];

    inv.data[7] = data[0] * data[6] * data[11] - 
             data[0] * data[7] * data[10] - 
             data[4] * data[2] * data[11] + 
             data[4] * data[3] * data[10] + 
             data[8] * data[2] * data[7] - 
             data[8] * data[3] * data[6];

    inv.data[11] = -data[0] * data[5] * data[11] + 
               data[0] * data[7] * data[9] + 
               data[4] * data[1] * data[11] - 
               data[4] * data[3] * data[9] - 
               data[8] * data[1] * data[7] + 
               data[8] * data[3] * data[5];

    inv.data[15] = data[0] * data[5] * data[10] - 
              data[0] * data[6] * data[9] - 
              data[4] * data[1] * data[10] + 
              data[4] * data[2] * data[9] + 
              data[8] * data[1] * data[6] - 
              data[8] * data[2] * data[5];

    float det = Determinant();

	if (det == 0.0f)
		return Matrix4();

    for (int i = 0; i < 16; ++i)
		inv.data[i] /= det;

	return inv;
}

Matrix4 Matrix4::Transpose(void)const
{
	Matrix4 t = *this;
	t.mtrx[0][1] = mtrx[1][0];
	t.mtrx[0][2] = mtrx[2][0];
	t.mtrx[0][3] = mtrx[3][0];
	t.mtrx[1][0] = mtrx[0][1];
	t.mtrx[1][2] = mtrx[2][1];
	t.mtrx[1][3] = mtrx[3][1];
	t.mtrx[2][0] = mtrx[0][2];
	t.mtrx[2][1] = mtrx[1][2];
	t.mtrx[2][3] = mtrx[3][2];
	t.mtrx[3][0] = mtrx[0][3];
	t.mtrx[3][1] = mtrx[1][3];
	t.mtrx[3][2] = mtrx[2][3];
	return t;
}

const Matrix4& Matrix4::operator+=(const Matrix4& rhs)
{
	data[0] += rhs.data[0];
	data[1] += rhs.data[1];
	data[2] += rhs.data[2];
	data[3] += rhs.data[3];

	data[4] += rhs.data[4];
	data[5] += rhs.data[5];
	data[6] += rhs.data[6];
	data[7] += rhs.data[7];
	
	data[8] += rhs.data[8];
	data[9] += rhs.data[9];
	data[10] += rhs.data[10];
	data[11] += rhs.data[11];
	
	data[12] += rhs.data[12];
	data[13] += rhs.data[13];
	data[14] += rhs.data[14];
	data[15] += rhs.data[15];
	return *this;
}

const Matrix4& Matrix4::operator*=(const Matrix4& rhs)
{
	return *this = *this*rhs;
}

Matrix4 Matrix4::operator*(float rhs)const
{
	Matrix4 ret(*this);
	for (size_t i=0;i<16;++i)
	{
		ret.data[i]*=rhs;
	}
	return ret;
}

Matrix4 Matrix4::operator/(float rhs)const
{
	Matrix4 ret(*this);
	for (size_t i=0;i<16;++i)
		ret.data[i]/=rhs;
	return ret;
}

const Matrix4& Matrix4::operator*=(float rhs)
{
	for (int i=0;i<16;++i)
		data[i]*=rhs;
	return *this;
}

const Matrix4& Matrix4::operator/=(float rhs)
{
	for (int i=0;i<16;++i)
		data[i]/=rhs;
	return *this;
}

bool Matrix4::operator!=(const Matrix4& rhs)const {
	bool ne = false;
	for (int i=0;i<16;++i)
		ne |= data[i]!=rhs.data[i];
	return ne;
}

bool Matrix4::operator==(const Matrix4& rhs)const {
	bool e = true;
	for (int i=0;i<16;++i)
		e &= data[i]==rhs.data[i];
	return e;
}

Matrix4 Matrix4::Translation(const Vec3& v)
{
	Matrix4 ret;
	ret.data[12] = v.x;
	ret.data[13] = v.y;
	ret.data[14] = v.z;
	return ret;
}

Matrix4 Matrix4::Scale(const Vec3& v)
{
	Matrix4 ret;
	ret.data[0] = v.x;
	ret.data[5] = v.y;
	ret.data[10] = v.z;
	return ret;
}

Matrix4 Matrix4::Perspective(float fov, float aspect, float near_z, float far_z)
{
	float depth_z = far_z - near_z;
	float ood = 1.0f / depth_z;

	Matrix4 pers;
	pers.mtrx[1][1] = 1.0f / tan(fov / 360.0f * M_PI);
	pers.mtrx[0][0] = -pers.mtrx[1][1] / aspect;
	pers.mtrx[2][2] = far_z * ood;
	pers.mtrx[3][2] = -far_z * near_z * ood;
	pers.mtrx[2][3] = 1.0f;
	pers.mtrx[3][3] = 0.0f;
	return pers;
}

outstream& operator<<(outstream& os, const Matrix4& m)
{
	os << m.data[0] << m.data[1] << m.data[2] << m.data[3] <<
		m.data[4] << m.data[5] << m.data[6] << m.data[7] <<
		m.data[8] << m.data[9] << m.data[10] << m.data[11] <<
		m.data[12] << m.data[13] << m.data[14] << m.data[15];
	return os;
}

instream& operator>>(instream& is, Matrix4& m)
{
	is >> m.data[0] >> m.data[1] >> m.data[2] >> m.data[3] >>
		m.data[4] >> m.data[5] >> m.data[6] >> m.data[7] >>
		m.data[8] >> m.data[9] >> m.data[10] >> m.data[11] >>
		m.data[12] >> m.data[13] >> m.data[14] >> m.data[15];
	return is;
}