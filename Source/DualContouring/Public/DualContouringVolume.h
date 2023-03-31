#pragma once

#include "CoreMinimal.h"
#include "DualContouringVolume.generated.h"

class SDF;

USTRUCT()
struct FDualContouringVolume
{
	GENERATED_BODY()
	
	SDF* sdf;
	
	uint32 size; // e.g. 32 = 32x32x32 voxels
	uint32 sampleSize; // e.g. 1 = 32x32x32 detail, 2 = 16x16x16 detail
	
	FDualContouringVolume() {}
	FDualContouringVolume(SDF* sdf, uint32 size, uint32 sampleSize) {}
};