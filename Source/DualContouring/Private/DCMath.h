#pragma once

#include "CoreMinimal.h"

struct SMat3
{
	double m00, m01, m02, m11, m12, m22;

	SMat3();
	SMat3(double m00, double m01, double m02, double m11, double m12, double m22);
	SMat3(const SMat3& rhs);

	void Clear();
	void SetSymmetric(double a00, double a01, double a02, double a11, double a12, double a22);
	void SetSymmetric(const SMat3& rhs);
};

struct Mat3
{
	double m00, m01, m02, m10, m11, m12, m20, m21, m22;

	Mat3();
	Mat3(double m00, double m01, double m02, double m10, double m11, double m12, double m20, double m21, double m22);
	Mat3(const Mat3& rhs);

	void Clear();
	void Set(double m00, double m01, double m02, double m10, double m11, double m12, double m20, double m21, double m22);
	void Set(const Mat3& rhs);

	void SetSymmetric(double a00, double a01, double a02, double a11, double a12, double a22);
	void SetSymmetric(const SMat3& rhs);
};

// Math functions
inline void vmul_symmetric(FVector& out, SMat3 a, FVector v)
{
	out = FVector((a.m00 * v.X) + (a.m01 * v.Y) + (a.m02 * v.Z),
		(a.m01 * v.X) + (a.m11 * v.Y) + (a.m12 * v.Z),
		(a.m02 * v.X) + (a.m12 * v.Y) + (a.m22 * v.Z));
}

inline double pinv(double x, double tol)
{
	return (FMath::Abs(x) < tol || FMath::Abs(1 / x) < tol) ? 0.0 : (1.0 / x);
}

inline void PseudoInverse(Mat3& out, SMat3 d, Mat3 v, double tol)
{
	double d0 = pinv(d.m00, tol), d1 = pinv(d.m11, tol), d2 = pinv(d.m22, tol);

	out.Set(v.m00 * d0 * v.m00 + v.m01 * d1 * v.m01 + v.m02 * d2 * v.m02,
		v.m00 * d0 * v.m10 + v.m01 * d1 * v.m11 + v.m02 * d2 * v.m12,
		v.m00 * d0 * v.m20 + v.m01 * d1 * v.m21 + v.m02 * d2 * v.m22,
		v.m10 * d0 * v.m00 + v.m11 * d1 * v.m01 + v.m12 * d2 * v.m02,
		v.m10 * d0 * v.m10 + v.m11 * d1 * v.m11 + v.m12 * d2 * v.m12,
		v.m10 * d0 * v.m20 + v.m11 * d1 * v.m21 + v.m12 * d2 * v.m22,
		v.m20 * d0 * v.m00 + v.m21 * d1 * v.m01 + v.m22 * d2 * v.m02,
		v.m20 * d0 * v.m10 + v.m21 * d1 * v.m11 + v.m22 * d2 * v.m12,
		v.m20 * d0 * v.m20 + v.m21 * d1 * v.m21 + v.m22 * d2 * v.m22);
}

inline double fnorm(SMat3 a)
{
	return FMath::Sqrt((a.m00 * a.m00) + (a.m01 * a.m01) + (a.m02 * a.m02) + (a.m01 * a.m01) + (a.m11 * a.m11) + (a.m12 * a.m12) + (a.m02 * a.m02) + (a.m12 * a.m12) + (a.m22 * a.m22));
}

inline void calcSymmetricGivensCoefficients(double a_pp, double a_pq, double a_qq, double& c, double& s)
{
	if (a_pq == 0)
	{
		c = 1;
		s = 0;
		return;
	}

	double tau = (a_qq - a_pp) / (2.0 * a_pq);
	double stt = FMath::Sqrt(1.0 + tau * tau);
	double tan = 1.0 / ((tau >= 0) ? (tau + stt) : (tau - stt));

	c = 1.0 / FMath::Sqrt(1.0 + tan * tan);
	s = tan * c;
}

//inline double solveLeastSquares(PlanetMesh::Mat3 a, FVector b, FVector x, double svd_tol, int svd_sweeps, double pinv_tol) {}

inline void rot01(SMat3& m, double c, double s)
{
	calcSymmetricGivensCoefficients(m.m00, m.m01, m.m11, c, s);
	double cc = c * c;
	double ss = s * s;
	double mix = 2 * c * s * m.m01;

	m.SetSymmetric(cc * m.m00 - mix + ss * m.m11, 0, c * m.m02 - s * m.m12, ss * m.m00 + mix + cc * m.m11, s * m.m02 + c * m.m12, m.m22);
}

inline void rot01_post(Mat3& m, double c, double s)
{
	m.Set(c * m.m00 - s * m.m01, s * m.m00 + c * m.m01, m.m02, c * m.m10 - s * m.m11, s * m.m10 + c * m.m11, m.m12, c * m.m20 - s * m.m21, s * m.m20 + c * m.m21, m.m22);
}

inline void rot02(SMat3& m, double c, double s)
{
	calcSymmetricGivensCoefficients(m.m00, m.m02, m.m22, c, s);
	double cc = c * c;
	double ss = s * s;
	double mix = 2 * c * s * m.m02;

	m.SetSymmetric(cc * m.m00 - mix + ss * m.m22, c * m.m01 - s * m.m12, 0, m.m11, s * m.m01 + c * m.m12, ss * m.m00 + mix + cc * m.m22);
}

inline void rot02_post(Mat3& m, double c, double s)
{
	m.Set(c * m.m00 - s * m.m02, m.m01, s * m.m00 + c * m.m02, c * m.m10 - s * m.m12, m.m11, s * m.m10 + c * m.m12, c * m.m20 - s * m.m22, m.m21, s * m.m20 + c * m.m22);
}

inline void rot12(SMat3& m, double c, double s)
{
	calcSymmetricGivensCoefficients(m.m11, m.m12, m.m22, c, s);

	double cc = c * c;
	double ss = s * s;
	double mix = 2 * c * s * m.m12;

	m.SetSymmetric(m.m00, c * m.m01 - s * m.m02, s * m.m01 + c * m.m02, cc * m.m11 - mix + ss * m.m22, 0, ss * m.m11 + mix + cc * m.m22);
}

inline void rot12_post(Mat3& m, double c, double s)
{
	m.Set(m.m00, c * m.m01 - s * m.m02, s * m.m01 + c * m.m02, m.m10, c * m.m11 - s * m.m12, s * m.m11 + c * m.m12, m.m20, c * m.m21 - s * m.m22, s * m.m21 + c * m.m22);
}

inline void rotate01(SMat3& vtav, Mat3& v)
{
	if (vtav.m01 == 0)
		return;

	double c = 0, s = 0;
	rot01(vtav, c, s);
	rot01_post(v, c, s);
}

inline void rotate02(SMat3& vtav, Mat3& v)
{
	if (vtav.m02 == 0)
		return;

	double c = 0, s = 0;
	rot02(vtav, c, s);
	rot02_post(v, c, s);
}

inline void rotate12(SMat3& vtav, Mat3& v)
{
	if (vtav.m12 == 0)
		return;

	double c = 0, s = 0;
	rot12(vtav, c, s);
	rot12_post(v, c, s);
}

inline double off(SMat3 a)
{
	return FMath::Sqrt(2 * ((a.m01 * a.m01) + (a.m02 * a.m02) + (a.m12 * a.m12)));
}

inline void GetSymmetricSvd(SMat3 a, SMat3& vtav, Mat3& v, double tol, int max_sweeps)
{
	vtav.SetSymmetric(a);
	v.Set(1, 0, 0, 0, 1, 0, 0, 0, 1);

	double delta = tol * fnorm(vtav);

	for (int i = 0; i < max_sweeps && off(vtav) > delta; ++i)
	{
		rotate01(vtav, v);
		rotate02(vtav, v);
		rotate12(vtav, v);
	}
}

inline void vmul(FVector& out, Mat3 a, FVector v)
{
	out = FVector((a.m00 * v.X) + (a.m01 * v.Y) + (a.m02 * v.Z),
		(a.m10 * v.X) + (a.m11 * v.Y) + (a.m12 * v.Z),
		(a.m20 * v.X) + (a.m21 * v.Y) + (a.m22 * v.Z));
}

inline double calcError(Mat3 A, FVector x, FVector b)
{
	FVector vtmp;
	vmul(vtmp, A, x);
	vtmp = b - vtmp;
	return FVector::DotProduct(vtmp, vtmp);
}

inline double calcError(SMat3 origA, FVector x, FVector b)
{
	Mat3 A;
	FVector vtmp;
	A.SetSymmetric(origA);
	return calcError(A, x, b);
}

inline double SolveSymmetric(SMat3 A, FVector b, FVector x, double svd_tol, int svd_sweeps, double pinv_tol)
{
	Mat3 pinv;
	Mat3 V;
	SMat3 VTAV;
	GetSymmetricSvd(A, VTAV, V, svd_tol, svd_sweeps);
	PseudoInverse(pinv, VTAV, V, pinv_tol);
	vmul(x, pinv, b);
	return calcError(A, x, b);
}