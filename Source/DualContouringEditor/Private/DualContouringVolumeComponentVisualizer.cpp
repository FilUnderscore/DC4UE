#include "DualContouringVolumeComponentVisualizer.h"
#include "DualContouringVolumeComponent.h"

void FDualContouringVolumeComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if(const UDualContouringVolumeComponent* DCVolumeComponent = Cast<UDualContouringVolumeComponent>(Component))
	{
		const FVector Location = DCVolumeComponent->GetComponentLocation();
		const double halfSize = DCVolumeComponent->halfSize;

		const FQuat Rotation = DCVolumeComponent->GetComponentQuat();

		// Back Top Left/Right
		FVector BTL = Rotation.RotateVector(FVector(Location.X - halfSize, Location.Y - halfSize, Location.Z + halfSize));
		FVector BTR = Rotation.RotateVector(FVector(Location.X + halfSize, Location.Y - halfSize, Location.Z + halfSize));

		// Back Bottom Left/Right
		FVector BBL = Rotation.RotateVector(FVector(Location.X - halfSize, Location.Y - halfSize, Location.Z - halfSize));
		FVector BBR = Rotation.RotateVector(FVector(Location.X + halfSize, Location.Y - halfSize, Location.Z - halfSize));

		// Front Top Left/Right
		FVector FTL = Rotation.RotateVector(FVector(Location.X - halfSize, Location.Y + halfSize, Location.Z + halfSize));
		FVector FTR = Rotation.RotateVector(FVector(Location.X + halfSize, Location.Y + halfSize, Location.Z + halfSize));

		// Front Bottom Left/Right
		FVector FBL = Rotation.RotateVector(FVector(Location.X - halfSize, Location.Y + halfSize, Location.Z - halfSize));
		FVector FBR = Rotation.RotateVector(FVector(Location.X + halfSize, Location.Y + halfSize, Location.Z - halfSize));

		const FLinearColor Color = FLinearColor::Red;
		const float Thickness = 0.06 * (float)DCVolumeComponent->halfSize;
		PDI->DrawLine(BTL, BTR, Color, SDPG_Foreground, Thickness);
		PDI->DrawLine(BTL, BBL, Color, SDPG_Foreground, Thickness);
		PDI->DrawLine(BBL, BBR, Color, SDPG_Foreground, Thickness);
		PDI->DrawLine(BBR, BTR, Color, SDPG_Foreground, Thickness);

		PDI->DrawLine(BTL, FTL, Color, SDPG_Foreground, Thickness);
		PDI->DrawLine(BBL, FBL, Color, SDPG_Foreground, Thickness);

		PDI->DrawLine(FTL, FTR, Color, SDPG_Foreground, Thickness);
		PDI->DrawLine(FTL, FBL, Color, SDPG_Foreground, Thickness);
		PDI->DrawLine(FBL, FBR, Color, SDPG_Foreground, Thickness);
		PDI->DrawLine(FBR, FTR, Color, SDPG_Foreground, Thickness);

		PDI->DrawLine(BTR, FTR, Color, SDPG_Foreground, Thickness);
		PDI->DrawLine(BBR, FBR, Color, SDPG_Foreground, Thickness);
	}
}