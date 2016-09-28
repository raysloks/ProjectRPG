#include "Matrix3.h"
#include "Matrix4.h"

Matrix3::Matrix3(void)
{
	data[0] = 1.0f;
	data[1] = 0.0f;
	data[2] = 0.0f;

	data[3] = 0.0f;
	data[4] = 1.0f;
	data[5] = 0.0f;

	data[6] = 0.0f;
	data[7] = 0.0f;
	data[8] = 1.0f;
}

Matrix3::Matrix3(float angle, const Vec3& axis)
{
	Vec3 axis2 = Vec3(axis.x*axis.x, axis.y*axis.y, axis.z*axis.z);
	float L = axis2.x+axis2.y+axis2.z;

	data[0] = (axis2.x + (axis2.y + axis2.z) * cos(angle)) / L;
    data[1] = (axis.x * axis.y * (1 - cos(angle)) - axis.z * sqrt(L) * sin(angle)) / L;
    data[2] = (axis.x * axis.z * (1 - cos(angle)) + axis.y * sqrt(L) * sin(angle)) / L;

    data[3] = (axis.x * axis.y * (1 - cos(angle)) + axis.z * sqrt(L) * sin(angle)) / L;
    data[4] = (axis2.y + (axis2.x + axis2.z) * cos(angle)) / L;
    data[5] = (axis.y * axis.z * (1 - cos(angle)) - axis.x * sqrt(L) * sin(angle)) / L;

    data[6] = (axis.x * axis.z * (1 - cos(angle)) - axis.y * sqrt(L) * sin(angle)) / L;
    data[7] = (axis.y * axis.z * (1 - cos(angle)) + axis.x * sqrt(L) * sin(angle)) / L;
    data[8] = (axis2.z + (axis2.x + axis2.y) * cos(angle)) / L;
}

Matrix3::Matrix3(const Vec3& x, const Vec3& y, const Vec3& z)
{
	data[0] = x.x;
	data[1] = x.y;
	data[2] = x.z;

	data[3] = y.x;
	data[4] = y.y;
	data[5] = y.z;

	data[6] = z.x;
	data[7] = z.y;
	data[8] = z.z;
}

Matrix3::Matrix3(const Matrix4& mtrx)
{
	data[0] = mtrx.data[0];
	data[1] = mtrx.data[1];
	data[2] = mtrx.data[2];

	data[3] = mtrx.data[4];
	data[4] = mtrx.data[5];
	data[5] = mtrx.data[6];

	data[6] = mtrx.data[8];
	data[7] = mtrx.data[9];
	data[8] = mtrx.data[10];
}

Matrix3::~Matrix3(void)
{
}

float Matrix3::Determinant(void)const
{
	return data[0]*(data[4]*data[8]-data[5]*data[7])-data[1]*(data[8]*data[3]-data[5]*data[6])+data[2]*(data[3]*data[7]-data[4]*data[6]);
}

Matrix3 Matrix3::Inverse(void)const
{
	Matrix3 ret;

	ret.data[0] = data[4]*data[8]-data[5]*data[7];//48-57
	ret.data[1] = data[2]*data[7]-data[1]*data[8];//27-18
	ret.data[2] = data[1]*data[5]-data[2]*data[4];//15-24
	
	ret.data[3] = data[5]*data[6]-data[3]*data[8];//56-38
	ret.data[4] = data[0]*data[8]-data[2]*data[6];//08-26
	ret.data[5] = data[2]*data[3]-data[0]*data[5];//23-05
	
	ret.data[6] = data[3]*data[7]-data[4]*data[6];//37-46
	ret.data[7] = data[1]*data[6]-data[0]*data[7];//16-07
	ret.data[8] = data[0]*data[4]-data[1]*data[3];//04-13

	ret/=Determinant();

	return ret;
}

Matrix3 Matrix3::Transpose(void)const
{
	Matrix3 ret;

	ret.data[0] = data[0];
	ret.data[1] = data[3];
	ret.data[2] = data[6];
	
	ret.data[3] = data[1];
	ret.data[4] = data[4];
	ret.data[5] = data[7];
	
	ret.data[6] = data[2];
	ret.data[7] = data[5];
	ret.data[8] = data[8];

	return ret;
}

const Matrix3& Matrix3::operator*=(float rhs)
{
	data[0]*=rhs;
	data[1]*=rhs;
	data[2]*=rhs;

	data[3]*=rhs;
	data[4]*=rhs;
	data[5]*=rhs;

	data[6]*=rhs;
	data[7]*=rhs;
	data[8]*=rhs;

	return *this;
}

const Matrix3& Matrix3::operator/=(float rhs)
{
	if (rhs!=0.0f)
	{
		data[0]/=rhs;
		data[1]/=rhs;
		data[2]/=rhs;

		data[3]/=rhs;
		data[4]/=rhs;
		data[5]/=rhs;

		data[6]/=rhs;
		data[7]/=rhs;
		data[8]/=rhs;
	}

	return *this;
}

Matrix3 Matrix3::operator+(const Matrix3& rhs)const
{
	Matrix3 ret;
	ret.data[0] = data[0]+rhs.data[0];
	ret.data[1] = data[1]+rhs.data[1];
	ret.data[2] = data[2]+rhs.data[2];

	ret.data[3] = data[3]+rhs.data[3];
	ret.data[4] = data[4]+rhs.data[4];
	ret.data[5] = data[5]+rhs.data[5];
	
	ret.data[6] = data[6]+rhs.data[6];
	ret.data[7] = data[7]+rhs.data[7];
	ret.data[8] = data[8]+rhs.data[8];
	return ret;
}

Matrix3 Matrix3::operator*(float rhs)const
{
	Matrix3 ret;
	ret.data[0] = data[0]*rhs;
	ret.data[1] = data[1]*rhs;
	ret.data[2] = data[2]*rhs;

	ret.data[3] = data[3]*rhs;
	ret.data[4] = data[4]*rhs;
	ret.data[5] = data[5]*rhs;
	
	ret.data[6] = data[6]*rhs;
	ret.data[7] = data[7]*rhs;
	ret.data[8] = data[8]*rhs;
	return ret;
}

Matrix3 Matrix3::operator*(const Matrix3& rhs)const
{
	Matrix3 ret;
	ret.data[0] = data[0]*rhs.data[0]+data[1]*rhs.data[3]+data[2]*rhs.data[6];
	ret.data[1] = data[0]*rhs.data[1]+data[1]*rhs.data[4]+data[2]*rhs.data[7];
	ret.data[2] = data[0]*rhs.data[2]+data[1]*rhs.data[5]+data[2]*rhs.data[8];

	ret.data[3] = data[3]*rhs.data[0]+data[4]*rhs.data[3]+data[5]*rhs.data[6];
	ret.data[4] = data[3]*rhs.data[1]+data[4]*rhs.data[4]+data[5]*rhs.data[7];
	ret.data[5] = data[3]*rhs.data[2]+data[4]*rhs.data[5]+data[5]*rhs.data[8];

	ret.data[6] = data[6]*rhs.data[0]+data[7]*rhs.data[3]+data[8]*rhs.data[6];
	ret.data[7] = data[6]*rhs.data[1]+data[7]*rhs.data[4]+data[8]*rhs.data[7];
	ret.data[8] = data[6]*rhs.data[2]+data[7]*rhs.data[5]+data[8]*rhs.data[8];
	return ret;
}

Vec3 Matrix3::operator*(const Vec3& rhs)const
{
	return Vec3(rhs.x*data[0]+rhs.y*data[3]+rhs.z*data[6], rhs.x*data[1]+rhs.y*data[4]+rhs.z*data[7], rhs.x*data[2]+rhs.y*data[5]+rhs.z*data[8]);
}

bool Matrix3::operator==(const Matrix3& rhs)const
{
	return data[0]==rhs.data[0] && data[1]==rhs.data[1] && data[2]==rhs.data[2] &&
		data[3]==rhs.data[3] && data[4]==rhs.data[4] && data[5]==rhs.data[5] &&
		data[6]==rhs.data[6] && data[7]==rhs.data[7] && data[8]==rhs.data[8];
}

bool Matrix3::operator!=(const Matrix3& rhs)const
{
	if (data[0]!=rhs.data[0])
		return true;
	if (data[1]!=rhs.data[1])
		return true;
	if (data[2]!=rhs.data[2])
		return true;
	if (data[3]!=rhs.data[3])
		return true;
	if (data[4]!=rhs.data[4])
		return true;
	if (data[5]!=rhs.data[5])
		return true;
	if (data[6]!=rhs.data[6])
		return true;
	if (data[7]!=rhs.data[7])
		return true;
	if (data[8]!=rhs.data[8])
		return true;
	return false;
}

outstream& operator<<(outstream& os, const Matrix3& mtrx)
{
	os << mtrx.data[0] << mtrx.data[1] << mtrx.data[2]
	<< mtrx.data[3] << mtrx.data[4] << mtrx.data[5]
	<< mtrx.data[6] << mtrx.data[7] << mtrx.data[8];
	return os;
}

instream& operator>>(instream& is, Matrix3& mtrx)
{
	is >> mtrx.data[0] >> mtrx.data[1] >> mtrx.data[2]
	>> mtrx.data[3] >> mtrx.data[4] >> mtrx.data[5]
	>> mtrx.data[6] >> mtrx.data[7] >> mtrx.data[8];
	return is;
}