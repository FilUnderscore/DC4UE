#pragma once

#include "CoreMinimal.h"
#include "DCMath.h"

struct QEFData
{
	double ata_00, ata_01, ata_02, ata_11, ata_12, ata_22;
	double atb_x, atb_y, atb_z;
	double btb;
	double massPoint_x, massPoint_y, massPoint_z;
	int numPoints;

	QEFData();
	QEFData(const QEFData& rhs);
	QEFData(double ata_00, double ata_01, double ata_02, double ata_11, double ata_12, double ata_22, double atb_x, double atb_y, double atb_z, double btb, double massPoint_x, double massPoint_y, double massPoint_z, int numPoints);

	void Add(const QEFData& rhs);
	void Clear();
	void Set(double ata_00, double ata_01, double ata_02, double ata_11, double ata_12, double ata_22, double atb_x, double atb_y, double atb_z, double btb, double massPoint_x, double massPoint_y, double massPoint_z, int numPoints);
	void Set(const QEFData& rhs);
};

struct QEFSolver
{
	QEFData data;
	SMat3 ata;
	FVector atb, massPoint, x;
	bool hasSolution;

	QEFSolver();

	FVector GetMassPoint();

	void Add(double px, double py, double pz, double nx, double ny, double nz);
	void Add(FVector p, FVector n);
	void Add(const QEFData& rhs);

	double GetError();
	double GetError(FVector pos);

	void Reset();
	double Solve(FVector& outX, double svd_tol, int svd_sweeps, double pinv_tol);

	void SetAta();
	void SetAtb();
};