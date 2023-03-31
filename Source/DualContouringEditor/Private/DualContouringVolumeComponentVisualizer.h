#pragma once

#include "ComponentVisualizer.h"

class DUALCONTOURINGEDITOR_API FDualContouringVolumeComponentVisualizer : public FComponentVisualizer
{
private:
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
};