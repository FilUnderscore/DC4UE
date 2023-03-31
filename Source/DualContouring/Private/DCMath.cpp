#include "DCMath.h"

// SMat3
SMat3::SMat3()
{
	Clear();
}

SMat3::SMat3(double m00, double m01, double m02, double m11, double m12, double m22)
{
	SetSymmetric(m00, m01, m02, m11, m12, m22);
}

SMat3::SMat3(const SMat3& rhs)
{
	SetSymmetric(rhs);
}

void SMat3::Clear()
{
	SetSymmetric(0, 0, 0, 0, 0, 0);
}

void SMat3::SetSymmetric(double a00, double a01, double a02, double a11, double a12, double a22)
{
	this->m00 = a00;
	this->m01 = a01;
	this->m02 = a02;
	this->m11 = a11;
	this->m12 = a12;
	this->m22 = a22;
}

void SMat3::SetSymmetric(const SMat3& rhs)
{
	SetSymmetric(rhs.m00, rhs.m01, rhs.m02, rhs.m11, rhs.m12, rhs.m22);
}

// Mat3
Mat3::Mat3()
{
	Clear();
}

Mat3::Mat3(double m00, double m01, double m02, double m10, double m11, double m12, double m20, double m21, double m22)
{
	Set(m00, m01, m02, m10, m11, m12, m20, m21, m22);
}

Mat3::Mat3(const Mat3& rhs)
{
	Set(rhs);
}

void Mat3::Clear()
{
	Set(0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void Mat3::Set(double _m00, double _m01, double _m02, double _m10, double _m11, double _m12, double _m20, double _m21, double _m22)
{
	this->m00 = _m00;
	this->m01 = _m01;
	this->m02 = _m02;
	this->m10 = _m10;
	this->m11 = _m11;
	this->m12 = _m12;
	this->m20 = _m20;
	this->m21 = _m21;
	this->m22 = _m22;
}

void Mat3::Set(const Mat3& rhs)
{
	Set(rhs.m00, rhs.m01, rhs.m02, rhs.m10, rhs.m11, rhs.m12, rhs.m20, rhs.m21, rhs.m22);
}

void Mat3::SetSymmetric(double a00, double a01, double a02, double a11, double a12, double a22)
{
	Set(a00, a01, a02, a01, a11, a12, a02, a12, a22);
}

void Mat3::SetSymmetric(const SMat3& rhs)
{
	SetSymmetric(rhs.m00, rhs.m01, rhs.m02, rhs.m11, rhs.m12, rhs.m22);
}