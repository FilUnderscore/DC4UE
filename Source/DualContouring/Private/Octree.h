#pragma once

#include "CoreMinimal.h"
#include "QEFSolver.h"
#include "SDF.h"

const int MATERIAL_AIR = 0;
const int MATERIAL_SOLID = 1;

const int edgevmap[12][2]
{
	// X axis
	{2, 4},
	{1, 5},
	{2, 6},
	{3, 7},

	// Y axis
	{0, 2},
	{1, 3},
	{4, 6},
	{5, 7},

	// Z axis
	{0, 1},
	{2, 3},
	{4, 5},
	{6, 7},
};

enum OctreeNodeType
{
	Node_None,
	Node_Internal,
	Node_Pseudo,
	Node_Leaf
};

struct OctreeDrawInfo
{
	int index;
	int corners;
	FVector position;
	FVector averageNormal;
	QEFData qef;

	OctreeDrawInfo();
};

struct OctreeNode
{
	SDF* sdf;

	OctreeNodeType type;
	FVector min;
	double size;

	OctreeNode* children[8];
	OctreeDrawInfo* drawInfo;

	OctreeNode();
	OctreeNode(const OctreeNodeType type);

	~OctreeNode();
};

OctreeNode* ConstructLeaf(OctreeNode* leaf);
OctreeNode* ConstructOctreeNodes(OctreeNode* node);

OctreeNode* BuildOctree(SDF* sdf, FVector min, int size, double threshold);
void DestroyOctree(OctreeNode* node);

OctreeNode* SimplifyOctree(OctreeNode* node, double threshold);

inline FVector CalculateSurfaceNormal(SDF* sdf, const FVector& p)
{
	// Determine surface normal based on rate of change of surface (derivative).

	//const double H = 0.001;
	//const double dx = sdf->GetDensityAt(p + FVector(H, 0, 0)) - sdf->GetDensityAt(p - FVector(H, 0, 0));
	//const double dy = sdf->GetDensityAt(p + FVector(0, H, 0)) - sdf->GetDensityAt(p - FVector(0, H, 0));
	//const double dz = sdf->GetDensityAt(p + FVector(0, 0, H)) - sdf->GetDensityAt(p - FVector(0, 0, H));

	/*
	const double H = 0.0001;
	const double fp = sdf->GetDensityAt(p);
	const double dx = sdf->GetDensityAt(p + FVector(H, 0, 0)) - fp;
	const double dy = sdf->GetDensityAt(p + FVector(0, H, 0)) - fp;
	const double dz = sdf->GetDensityAt(p + FVector(0, 0, H)) - fp;
	*/

	const double H = 0.0001;
	return FVector((FVector(1, -1, -1) * sdf->GetDensityAt(p + FVector(1, -1, -1) * H)) + 
				   (FVector(-1, -1, 1) * sdf->GetDensityAt(p + FVector(-1, -1, 1) * H)) + 
				   (FVector(-1, 1, -1) * sdf->GetDensityAt(p + FVector(-1, 1, -1) * H)) +
					(FVector(1, 1, 1) * sdf->GetDensityAt(p + FVector(1,1,1) * H)));

	//return FVector(dx, dy, dz).GetSafeNormal();
}

inline FVector ApproximateZeroCrossingPosition(SDF* sdf, const FVector& p0, const FVector& p1)
{
	double minValue = 100000.0;
	double t = 0.0;
	double currentT = 0.0;

	const int steps = 8;
	const double increment = 1.0 / (double)steps;

	while (currentT <= 1.0)
	{
		const FVector p = p0 + ((p1 - p0) * currentT);
		const float density = FMath::Abs(sdf->GetDensityAt(p));

		if (density < minValue)
		{
			minValue = density;
			t = currentT;
		}

		currentT += increment;
	}

	return p0 + ((p1 - p0) * t);
}