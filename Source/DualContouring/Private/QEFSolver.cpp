#include "QEFSolver.h"

// QEFData

QEFData::QEFData()
{
	Clear();
}

QEFData::QEFData(const QEFData& rhs)
{
	Set(rhs);
}

QEFData::QEFData(double ata_00, double ata_01, double ata_02, double ata_11, double ata_12, double ata_22, double atb_x, double atb_y, double atb_z, double btb, double massPoint_x, double massPoint_y, double massPoint_z, int numPoints)
{
	Set(ata_00, ata_01, ata_02, ata_11, ata_12, ata_22, atb_x, atb_y, atb_z, btb, massPoint_x, massPoint_y, massPoint_z, numPoints);
}

void QEFData::Add(const QEFData& rhs)
{
	ata_00 += rhs.ata_00;
	ata_01 += rhs.ata_01;
	ata_02 += rhs.ata_02;
	ata_11 += rhs.ata_11;
	ata_12 += rhs.ata_12;
	ata_22 += rhs.ata_22;
	atb_x += rhs.atb_x;
	atb_y += rhs.atb_y;
	atb_z += rhs.atb_z;
	btb += rhs.btb;
	massPoint_x += rhs.massPoint_x;
	massPoint_y += rhs.massPoint_y;
	massPoint_z += rhs.massPoint_z;
	numPoints += rhs.numPoints;
}

void QEFData::Clear()
{
	Set(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void QEFData::Set(double _ata_00, double _ata_01, double _ata_02, double _ata_11, double _ata_12, double _ata_22, double _atb_x, double _atb_y, double _atb_z, double _btb, double _massPoint_x, double _massPoint_y, double _massPoint_z, int _numPoints)
{
	this->ata_00 = _ata_00;
	this->ata_01 = _ata_01;
	this->ata_02 = _ata_02;
	this->ata_11 = _ata_11;
	this->ata_12 = _ata_12;
	this->ata_22 = _ata_22;
	this->atb_x = _atb_x;
	this->atb_y = _atb_y;
	this->atb_z = _atb_z;
	this->btb = _btb;
	this->massPoint_x = _massPoint_x;
	this->massPoint_y = _massPoint_y;
	this->massPoint_z = _massPoint_z;
	this->numPoints = _numPoints;
}

void QEFData::Set(const QEFData& rhs)
{
	Set(rhs.ata_00, rhs.ata_01, rhs.ata_02, rhs.ata_11, rhs.ata_12, rhs.ata_22, rhs.atb_x, rhs.atb_y, rhs.atb_z, rhs.btb, rhs.massPoint_x, rhs.massPoint_y, rhs.massPoint_z, rhs.numPoints);
}

// QEFSolver
QEFSolver::QEFSolver() : data(QEFData()), ata(SMat3()), atb(FVector::ZeroVector), massPoint(FVector::ZeroVector), x(FVector::ZeroVector), hasSolution(false) {}

FVector QEFSolver::GetMassPoint()
{
	return this->massPoint;
}

void QEFSolver::Add(double px, double py, double pz, double nx, double ny, double nz)
{
	this->hasSolution = false;

	FVector tmpv = FVector(nx, ny, nz).GetSafeNormal();
	nx = tmpv.X;
	ny = tmpv.Y;
	nz = tmpv.Z;

	data.ata_00 += nx * nx;
	data.ata_01 += nx * ny;
	data.ata_02 += nx * nz;
	data.ata_11 += ny * ny;
	data.ata_12 += ny * nz;
	data.ata_22 += nz * nz;

	float dot = nx * px + ny * py + nz * pz;
	data.atb_x += dot * nx;
	data.atb_y += dot * ny;
	data.atb_z += dot * nz;

	data.btb += dot * dot;

	data.massPoint_x += px;
	data.massPoint_y += py;
	data.massPoint_z += pz;

	++data.numPoints;
}

void QEFSolver::Add(FVector p, FVector n)
{
	Add(p.X, p.Y, p.Z, n.X, n.Y, n.Z);
}

void QEFSolver::Add(const QEFData& rhs)
{
	hasSolution = false;
	data.Add(rhs);
}

double QEFSolver::GetError()
{
	check(hasSolution);

	return GetError(x);
}

double QEFSolver::GetError(FVector pos)
{
	if (!hasSolution)
	{
		SetAta();
		SetAtb();
	}

	FVector atax;
	vmul_symmetric(atax, ata, pos);
	return FVector::DotProduct(pos, atax) - 2 * FVector::DotProduct(pos, atb) + data.btb;
}

void QEFSolver::Reset()
{
	hasSolution = false;
	data.Clear();
}

double QEFSolver::Solve(FVector& outX, double svd_tol, int svd_sweeps, double pinv_tol)
{
	check(data.numPoints > 0);

	massPoint.Set(data.massPoint_x, data.massPoint_y, data.massPoint_z);
	massPoint *= (1.0 / data.numPoints);
	SetAta();
	SetAtb();

	FVector tmpv;
	vmul_symmetric(tmpv, ata, massPoint);
	atb = atb - tmpv;
	x = FVector::ZeroVector;

	double result = SolveSymmetric(ata, atb, x, svd_tol, svd_sweeps, pinv_tol);
	x += massPoint * 1;
	SetAtb();

	outX = x;
	hasSolution = true;

	return result;
}

void QEFSolver::SetAta()
{
	ata.SetSymmetric(data.ata_00, data.ata_01, data.ata_02, data.ata_11, data.ata_12, data.ata_22);
}

void QEFSolver::SetAtb()
{
	atb.Set(data.atb_x, data.atb_y, data.atb_z);
}