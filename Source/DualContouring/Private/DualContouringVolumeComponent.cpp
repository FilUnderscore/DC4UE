#include "DualContouringVolumeComponent.h"
#include "SDF.h"
#include "Components/RuntimeMeshComponentStatic.h"

UDualContouringVolumeComponent::UDualContouringVolumeComponent() 
{
	PrimaryComponentTick.bCanEverTick = false;

	this->meshComponent = CreateDefaultSubobject<URuntimeMeshComponentStatic>(TEXT("DCVolumeMesh"));
	this->meshComponent->SetupAttachment(this);
}

void UDualContouringVolumeComponent::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR
	this->preview = true;
#endif

	//if (this->dirty)
		//this->Regenerate();
}

void UDualContouringVolumeComponent::BeginDestroy()
{
	Super::BeginDestroy();

	this->Clear();
}

#if WITH_EDITOR
void UDualContouringVolumeComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UDualContouringVolumeComponent, preview))
	{
		if (preview)
		{
			this->RegenerateMesh();
		}
		else
		{
			this->ClearMesh();
		}
	}
	else
	{
		this->dirty = true;
	}
}
#endif

void UDualContouringVolumeComponent::Regenerate()
{
	if (this->sdf == nullptr)
		return;

	this->Clear();

	this->roundedHalfSize = FMath::Pow(2, FMath::CeilLogTwo(FMath::Max(1, this->halfSize / (double)(this->levelOfDetail + 1))));
	int min = -(this->roundedHalfSize / 2); // 32, LOD = 0 -> 32 || 32, LOD = 1 -> 16
	
	UE_LOG(LogTemp, Warning, TEXT("HS %f LOD %d RHS %f"), this->halfSize, levelOfDetail, roundedHalfSize);

	DualContouring::Generate(this->sdf, FVector(min, min, min), roundedHalfSize, -1.0, vertexBuffer, indexBuffer);

	this->dirty = false;

	this->RegenerateMesh();
}

void UDualContouringVolumeComponent::RegenerateMesh()
{
#if WITH_EDITOR
	if (!preview)
		return;
#endif

	double diff = this->halfSize / FMath::Max(this->roundedHalfSize, 1);
	
	TArray<FVector2D> uvs;
	TArray<FLinearColor> vertexColors;
	TArray<FRuntimeMeshTangent> tangents;

	for (FVector& vertex : vertexBuffer.vertices)
	{
		vertex *= diff;
	}

	UE_LOG(LogTemp, Warning, TEXT("Vertex count %d Tri count %d"), vertexBuffer.vertices.Num(), indexBuffer.Num());

	uvs.Init(FVector2D(0, 0), vertexBuffer.vertices.Num());
	vertexColors.Init(FLinearColor(1.0, 1.0, 1.0, 1.0), vertexBuffer.vertices.Num());
	tangents.Init(FRuntimeMeshTangent(1.0, 0, 0), vertexBuffer.vertices.Num());

	if (!meshVisible)
	{
		this->meshComponent->CreateSectionFromComponents(0, 0, 0, vertexBuffer.vertices, indexBuffer, vertexBuffer.normals, uvs, vertexColors, tangents, ERuntimeMeshUpdateFrequency::Average, true);
		meshVisible = true;
	}
	else
	{
		this->meshComponent->UpdateSectionFromComponents(0, 0, vertexBuffer.vertices, indexBuffer, vertexBuffer.normals, uvs, vertexColors, tangents);
	}
}

void UDualContouringVolumeComponent::Clear()
{
	this->vertexBuffer.vertices.Empty();
	this->vertexBuffer.normals.Empty();

	this->indexBuffer.Empty();

	this->ClearMesh();
}

void UDualContouringVolumeComponent::ClearMesh()
{
	if (!meshVisible)
		return;

	this->meshComponent->RemoveSection(0, 0);
	meshVisible = false;
}

void UDualContouringVolumeComponent::SetSize(double qualitySize, double actualSize)
{
	this->halfSize = actualSize;
	this->levelOfDetail = FMath::CeilToInt((actualSize / qualitySize)) - 1;

	UE_LOG(LogTemp, Warning, TEXT("Size: %f LOD %d"), this->halfSize, this->levelOfDetail);
}