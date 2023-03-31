#include "Octree.h"

OctreeDrawInfo::OctreeDrawInfo() : index(-1), corners(0) {}

OctreeNode::OctreeNode() : OctreeNode(Node_None)
{
	for (int i = 0; i < 8; i++)
		children[i] = nullptr;
}

OctreeNode::OctreeNode(const OctreeNodeType type) : type(type), min(0, 0, 0), size(0), drawInfo(nullptr)
{
	for (int i = 0; i < 8; i++)
		children[i] = nullptr;
}

OctreeNode::~OctreeNode()
{
	if (this->drawInfo != nullptr)
		delete this->drawInfo;
}

const FVector CHILD_MIN_OFFSETS[] =
{
	FVector(0, 0, 0),
	FVector(0, 0, 1),
	FVector(0, 1, 0),
	FVector(0, 1, 1),
	FVector(1, 0, 0),
	FVector(1, 0, 1),
	FVector(1, 1, 0),
	FVector(1, 1, 1),
};

const double QEF_ERROR = 1e-6f;
const int QEF_SWEEPS = 4;

OctreeNode* ConstructLeaf(OctreeNode* leaf)
{
	//if (leaf == nullptr || leaf->size != 1)
	if(leaf == nullptr)
	{
		return nullptr;
	}

	int corners = 0;
	for (int i = 0; i < 8; i++)
	{
		FVector cornerPos = leaf->min + CHILD_MIN_OFFSETS[i];
		double density = leaf->sdf->GetDensityAt(cornerPos);

		int material = density < 0.0 ? MATERIAL_SOLID : MATERIAL_AIR;
		corners |= (material << i);
	}

	if (corners == 0 || corners == 255)
	{
		// Voxel is full inside or outside the volume.
		delete leaf;
		return nullptr;
	}

	const int MAX_CROSSINGS = 6;
	int edgeCount = 0;
	FVector averageNormal = FVector::ZeroVector;
	QEFSolver qefSolver;

	for (int i = 0; i < 12 && edgeCount < MAX_CROSSINGS; i++)
	{
		int c1 = edgevmap[i][0];
		int c2 = edgevmap[i][1];

		int m1 = (corners >> c1) & 1;
		int m2 = (corners >> c2) & 1;

		if ((m1 == MATERIAL_AIR && m2 == MATERIAL_AIR) || (m1 == MATERIAL_SOLID && m2 == MATERIAL_SOLID))
			continue;

		FVector p1 = leaf->min + CHILD_MIN_OFFSETS[c1];
		FVector p2 = leaf->min + CHILD_MIN_OFFSETS[c2];
		FVector p = ApproximateZeroCrossingPosition(leaf->sdf, p1, p2);
		FVector n = CalculateSurfaceNormal(leaf->sdf, p);
		
		qefSolver.Add(p.X, p.Y, p.Z, n.X, n.Y, n.Z);
		averageNormal += n;

		edgeCount++;
	}

	FVector qefPosition = FVector::ZeroVector;
	qefSolver.Solve(qefPosition, QEF_ERROR, QEF_SWEEPS, QEF_ERROR);

	OctreeDrawInfo* drawInfo = new OctreeDrawInfo;
	drawInfo->corners = 0;
	drawInfo->index = -1;
	drawInfo->position = qefPosition;
	drawInfo->qef = qefSolver.data;

	FVector min = leaf->min;
	FVector max = FVector(min.X + leaf->size, min.Y + leaf->size, min.Z + leaf->size);

	if (drawInfo->position.X < min.X || drawInfo->position.X > max.X ||
		drawInfo->position.Y < min.Y || drawInfo->position.Y > max.Y ||
		drawInfo->position.Z < min.Z || drawInfo->position.Z > max.Z)
	{
		drawInfo->position = qefSolver.GetMassPoint();
	}

	drawInfo->averageNormal = FVector(averageNormal / (double)edgeCount).GetSafeNormal();
	drawInfo-> corners = corners;

	leaf->type = Node_Leaf;
	leaf->drawInfo = drawInfo;

	return leaf;
}

OctreeNode* ConstructOctreeNodes(OctreeNode* node)
{
	if (!node)
		return nullptr;

	if (node->size == 1)
		return ConstructLeaf(node);

	const int childSize = node->size / 2;
	bool hasChildren = false;

	for (int i = 0; i < 8; i++)
	{
		/*
		OctreeNode* child = new OctreeNode;
		child->size = childSize;
		child->min = node->min + (CHILD_MIN_OFFSETS[i] * childSize);
		child->type = Node_Internal;
		child->sdf = node->sdf;

		node->children[i] = ConstructOctreeNodes(child);
		hasChildren |= (node->children[i] != nullptr);
		*/

		//const FVector centerPos = node->min + (CHILD_MIN_OFFSETS[i] * (childSize / 2));
		const FVector centerPos = node->min + ((CHILD_MIN_OFFSETS[i] + FVector(0.5, 0.5, 0.5)) * childSize);
		const double density = node->sdf->GetDensityAt(centerPos);
		constexpr double CornerConstant = 1.7320508075688772935274463415059 / 2;
		const bool corner = FMath::Abs(density) > childSize * CornerConstant;

		if (corner) // MATERIAL_AIR
		{
			// Skip.
			node->children[i] = nullptr;
		}
		else
		{
			// Subdivide.
			OctreeNode* child = new OctreeNode;
			child->size = childSize;
			child->min = node->min + (CHILD_MIN_OFFSETS[i] * childSize);
			child->type = Node_Internal;
			child->sdf = node->sdf;

			node->children[i] = ConstructOctreeNodes(child);
		}

		hasChildren |= (node->children[i] != nullptr);
	}

	if (!hasChildren)
	{
		delete node;
		return nullptr;
	}

	return node;
}

void DestroyOctree(OctreeNode* node)
{
	if (node == nullptr)
		return;

	for (int i = 0; i < 8; i++)
	{
		DestroyOctree(node->children[i]);
	}

	delete node;
	node = nullptr;
}

OctreeNode* SimplifyOctree(OctreeNode* node, double threshold)
{
	if (node == nullptr)
		return nullptr;

	if (node->type != Node_Internal) // Can't simplify.
		return node;

	QEFSolver qefSolver;
	int signs[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
	int midsign = -1;
	int edgeCount = 0;
	bool isCollapsible = true;

	for (int i = 0; i < 8; i++)
	{
		node->children[i] = SimplifyOctree(node->children[i], threshold);

		if (node->children[i] != nullptr)
		{
			OctreeNode* child = node->children[i];

			if (child->type == Node_Internal)
			{
				isCollapsible = false;
			}
			else
			{
				qefSolver.Add(child->drawInfo->qef);

				midsign = (child->drawInfo->corners >> (7 - i)) & 1;
				signs[i] = (child->drawInfo->corners >> i) & 1;

				edgeCount++;
			}
		}
	}

	if (!isCollapsible) // Atleast one child is an internal node, can't collapse.
		return node;

	FVector qefPosition = FVector::ZeroVector;
	qefSolver.Solve(qefPosition, QEF_ERROR, QEF_SWEEPS, QEF_ERROR);
	double error = qefSolver.GetError();

	FVector position = qefPosition;

	if (error > threshold)
		return node;

	FVector min = node->min;
	FVector max = FVector(min.X + node->size, min.Y + node->size, min.Z + node->size);

	if (position.X < min.X || position.X > max.X ||
		position.Y < min.Y || position.Y > max.Y ||
		position.Z < min.Z || position.Z > max.Z)
	{
		position = qefSolver.GetMassPoint();
	}

	OctreeDrawInfo* drawInfo = new OctreeDrawInfo;
	drawInfo->corners = 0;
	drawInfo->index = -1;

	for (int i = 0; i < 8; i++)
	{
		if (signs[i] == -1)
		{
			// Undetermined. Use center sign instead.
			drawInfo->corners |= (midsign << i);
		}
		else
		{
			drawInfo->corners |= (signs[i] << i);
		}
	}

	drawInfo->averageNormal = FVector::ZeroVector;
	for (int i = 0; i < 8; i++)
	{
		if (node->children[i] != nullptr)
		{
			OctreeNode* child = node->children[i];
			if (child->type == Node_Pseudo || child->type == Node_Leaf)
			{
				drawInfo->averageNormal += child->drawInfo->averageNormal;
			}
		}
	}

	drawInfo->averageNormal = drawInfo->averageNormal.GetSafeNormal();
	drawInfo->position = position;
	drawInfo->qef = qefSolver.data;

	for (int i = 0; i < 8; i++)
	{
		DestroyOctree(node->children[i]);
		node->children[i] = nullptr;
	}

	node->type = Node_Pseudo;
	node->drawInfo = drawInfo;

	return node;
}

OctreeNode* BuildOctree(SDF* sdf, FVector min, int size, double threshold)
{
	OctreeNode* root = new OctreeNode(Node_Internal);
	root->min = min;
	root->size = size;
	root->sdf = sdf;

	root = ConstructOctreeNodes(root);
	//root = SimplifyOctree(root, threshold);

	return root;
}