#pragma once

#include "CoreMinimal.h"

class SDF;

struct DCVertexBuffer
{
	TArray<FVector> vertices;
	TArray<FVector> normals;
};

typedef TArray<int> DCIndexBuffer;

class DUALCONTOURING_API DualContouring
{
public:
	static void Generate(SDF* sdf, FVector min, int size, double threshold, DCVertexBuffer& vertexBuffer, DCIndexBuffer& indexBuffer);
};