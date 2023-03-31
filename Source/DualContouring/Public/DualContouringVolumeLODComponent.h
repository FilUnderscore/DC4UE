#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "DualContouringVolumeLODComponent.generated.h"

class UDualContouringVolumeComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUALCONTOURING_API UDualContouringVolumeLODComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	UDualContouringVolumeComponent* volumeComponent;
	
public:
	UPROPERTY(EditAnywhere)
	TMap<unsigned int, double> levelOfDetailMinimumDistances;
	
	UDualContouringVolumeLODComponent();
};