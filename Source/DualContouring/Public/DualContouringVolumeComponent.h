#pragma once

#include "CoreMinimal.h"
#include "DualContouring.h"
#include "Components/SceneComponent.h"
#include "DualContouringVolumeComponent.generated.h"

class SDF;
class URuntimeMeshComponentStatic;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUALCONTOURING_API UDualContouringVolumeComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	bool dirty = true;
	bool meshVisible;
	URuntimeMeshComponentStatic* meshComponent;

	double roundedHalfSize;

	DCVertexBuffer vertexBuffer;
	DCIndexBuffer indexBuffer;

	void RegenerateMesh();

	void Clear();
	void ClearMesh();

public:
	UPROPERTY(EditAnywhere)
	double halfSize;
	
	UPROPERTY(EditAnywhere)
	int levelOfDetail;
	
	SDF* sdf = nullptr;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere)
	bool preview;
#endif

	UDualContouringVolumeComponent();

	void Regenerate();
	void SetSize(double qualitySize, double actualSize);

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};