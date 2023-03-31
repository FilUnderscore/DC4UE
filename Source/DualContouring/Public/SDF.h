#pragma once

#include "CoreMinimal.h"

class DUALCONTOURING_API SDF
{
public:
	virtual double GetDensityAt(FVector worldPos) PURE_VIRTUAL(SDF::GetDensityAt, return 0.0;);
};