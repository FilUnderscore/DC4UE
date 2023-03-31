#include "DualContouring.h"
#include "SDF.h"
#include "Octree.h"

void GenerateVertexIndices(OctreeNode* node, DCVertexBuffer& vertexBuffer)
{
	if (node == nullptr)
		return;

	if (node->type != Node_Leaf)
	{
		for (int i = 0; i < 8; i++)
		{
			GenerateVertexIndices(node->children[i], vertexBuffer);
		}
	}

	if (node->type != Node_Internal)
	{
		node->drawInfo->index = vertexBuffer.vertices.Num();

		vertexBuffer.vertices.Add(node->drawInfo->position);
		vertexBuffer.normals.Add(node->drawInfo->averageNormal);
	}
}

const int processEdgeMask[3][4] =
{
	{3, 2, 1, 0},
	{7, 5, 6, 4},
	{11, 10, 9, 8},
};

void ContourProcessEdge(OctreeNode* node[4], int dir, DCIndexBuffer& indexArray)
{
	int minSize = 1000000; // Arbitrary large number
	int minIndex = 0;
	int indices[4] = { -1, -1, -1, -1 };

	bool flip = false;
	bool signChange[4] = { false, false, false, false };

	for (int i = 0; i < 4; i++)
	{
		int edge = processEdgeMask[dir][i];
		int c1 = edgevmap[edge][0];
		int c2 = edgevmap[edge][1];

		int m1 = (node[i]->drawInfo->corners >> c1) & 1;
		int m2 = (node[i]->drawInfo->corners >> c2) & 1;

		if (node[i]->size < minSize)
		{
			minSize = node[i]->size;
			minIndex = i;
			flip = m1 != MATERIAL_AIR;
		}

		indices[i] = node[i]->drawInfo->index;

		signChange[i] = (m1 == MATERIAL_AIR && m2 != MATERIAL_AIR) ||
			(m1 != MATERIAL_AIR && m2 == MATERIAL_AIR); // TODO: Exclusive OR maybe?
	}

	if (signChange[minIndex])
	{
		if (flip)
		{
			indexArray.Add(indices[0]);
			indexArray.Add(indices[1]);
			indexArray.Add(indices[3]);

			indexArray.Add(indices[0]);
			indexArray.Add(indices[3]);
			indexArray.Add(indices[2]);
		}
		else
		{
			indexArray.Add(indices[0]);
			indexArray.Add(indices[3]);
			indexArray.Add(indices[1]);

			indexArray.Add(indices[0]);
			indexArray.Add(indices[2]);
			indexArray.Add(indices[3]);
		}
	}
}

const int edgeProcEdgeMask[3][2][5] =
{
	{
		{3, 2, 1, 0, 0},
		{7, 6, 5, 4, 0},
	},
	{
		{5, 1, 4, 0, 1},
		{7, 3, 6, 2, 1},
	},
	{
		{6, 4, 2, 0, 2},
		{7, 5, 3, 1, 2},
	},
};

void ContourEdgeProc(OctreeNode* node[4], int dir, DCIndexBuffer& indices)
{
	if (node[0] == nullptr || node[1] == nullptr || node[2] == nullptr || node[3] == nullptr)
		return;

	if (node[0]->type != Node_Internal &&
		node[1]->type != Node_Internal &&
		node[2]->type != Node_Internal &&
		node[3]->type != Node_Internal)
	{
		ContourProcessEdge(node, dir, indices);
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			OctreeNode* edgeNodes[4] = { nullptr, nullptr, nullptr, nullptr };
			int c[4] = { edgeProcEdgeMask[dir][i][0], edgeProcEdgeMask[dir][i][1], edgeProcEdgeMask[dir][i][2], edgeProcEdgeMask[dir][i][3] };

			for (int j = 0; j < 4; j++)
			{
				if (node[j]->type == Node_Leaf || node[j]->type == Node_Pseudo)
				{
					edgeNodes[j] = node[j];
				}
				else
				{
					edgeNodes[j] = node[j]->children[c[j]];
				}
			}

			ContourEdgeProc(edgeNodes, edgeProcEdgeMask[dir][i][4], indices);
		}
	}
}

const int faceProcFaceMask[3][4][3] =
{
	{
		{
			4, 0, 0
		},
		{
			5, 1, 0
		},
		{
			6, 2, 0
		},
		{
			7, 3, 0
		},
	},
	{
		{
			2, 0, 1
		},
		{
			6, 4, 1
		},
		{
			3, 1, 1
		},
		{
			7, 5, 1
		},
	},
	{
		{
			1, 0, 2
		},
		{
			3, 2, 2
		},
		{
			5, 4, 2
		},
		{
			7, 6, 2
		},
	},
};

const int faceProcEdgeMask[3][4][6] =
{
	{
		{ 1, 4, 0, 5, 1, 1 },
		{ 1, 6, 2, 7, 3, 1 },
		{ 0, 4, 6, 0, 2, 2 },
		{ 0, 5, 7, 1, 3, 2 },
	},
	{
		{ 0, 2, 3, 0, 1, 0 },
		{ 0, 6, 7, 4, 5, 0 },
		{ 1, 2, 0, 6, 4, 2 },
		{ 1, 3, 1, 7, 5, 2 },
	},
	{
		{ 1, 1, 0, 3, 2, 0 },
		{ 1, 5, 4, 7, 6, 0 },
		{ 0, 1, 5, 0, 4, 1 },
		{ 0, 3, 7, 2, 6, 1 },
	},
};

void ContourFaceProc(OctreeNode* node[2], int dir, DCIndexBuffer& indices)
{
	if (node[0] == nullptr || node[1] == nullptr)
		return;

	if (node[0]->type == Node_Internal ||
		node[1]->type == Node_Internal)
	{
		for (int i = 0; i < 4; i++)
		{
			OctreeNode* faceNodes[2] = { nullptr, nullptr };
			int c[2] = { faceProcFaceMask[dir][i][0], faceProcFaceMask[dir][i][1] };

			for (int j = 0; j < 2; j++)
			{
				if (node[j]->type != Node_Internal)
				{
					faceNodes[j] = node[j];
				}
				else
				{
					faceNodes[j] = node[j]->children[c[j]];
				}
			}

			ContourFaceProc(faceNodes, faceProcFaceMask[dir][i][2], indices);
		}

		int orders[2][4] =
		{
			{ 0, 0, 1, 1 },
			{ 0, 1, 0, 1 },
		};

		for (int i = 0; i < 4; i++)
		{
			OctreeNode* edgeNodes[4] = { nullptr, nullptr, nullptr, nullptr };
			int c[4] = { faceProcEdgeMask[dir][i][1], faceProcEdgeMask[dir][i][2], faceProcEdgeMask[dir][i][3], faceProcEdgeMask[dir][i][4] };

			int* order = orders[faceProcEdgeMask[dir][i][0]];

			for (int j = 0; j < 4; j++)
			{
				if (node[order[j]]->type == Node_Leaf ||
					node[order[j]]->type == Node_Pseudo)
				{
					edgeNodes[j] = node[order[j]];
				}
				else
				{
					edgeNodes[j] = node[order[j]]->children[c[j]];
				}
			}

			ContourEdgeProc(edgeNodes, faceProcEdgeMask[dir][i][5], indices);
		}
	}
}

const int cellProcFaceMask[12][3] =
{
	{0, 4, 0},
	{1, 5, 0},
	{2, 6, 0},
	{3, 7, 0},
	{0, 2, 1},
	{4, 6, 1},
	{1, 3, 1},
	{5, 7, 1},
	{0, 1, 2},
	{2, 3, 2},
	{4, 5, 2},
	{6, 7, 2},
};

const int cellProcEdgeMask[6][5] =
{
	{0, 1, 2, 3, 0},
	{4, 5, 6, 7, 0},
	{0, 4, 1, 5, 1},
	{2, 6, 3, 7, 1},
	{0, 2, 4, 6, 2},
	{1, 3, 5, 7, 2},
};

void ContourCellProc(OctreeNode* node, DCIndexBuffer& indices)
{
	if (node == nullptr)
		return;

	if (node->type == Node_Internal)
	{
		for (int i = 0; i < 8; i++)
		{
			ContourCellProc(node->children[i], indices);
		}

		for (int i = 0; i < 12; i++)
		{
			OctreeNode* faceNodes[2] = { nullptr, nullptr };
			int c[2] = { cellProcFaceMask[i][0], cellProcFaceMask[i][1] };

			faceNodes[0] = node->children[c[0]];
			faceNodes[1] = node->children[c[1]];

			ContourFaceProc(faceNodes, cellProcFaceMask[i][2], indices);
		}

		for (int i = 0; i < 6; i++)
		{
			OctreeNode* edgeNodes[4] = { nullptr, nullptr, nullptr, nullptr };
			int c[4] = { cellProcEdgeMask[i][0], cellProcEdgeMask[i][1], cellProcEdgeMask[i][2], cellProcEdgeMask[i][3] };

			for (int j = 0; j < 4; j++)
			{
				edgeNodes[j] = node->children[c[j]];
			}

			ContourEdgeProc(edgeNodes, cellProcEdgeMask[i][4], indices);
		}
	}
}

void DualContouring::Generate(SDF* sdf, FVector min, int size, double threshold, DCVertexBuffer& vertexBuffer, DCIndexBuffer& indexBuffer)
{
	OctreeNode* node = BuildOctree(sdf, min, size, threshold);
	
	GenerateVertexIndices(node, vertexBuffer);
	ContourCellProc(node, indexBuffer);

	// TODO: Temporary until we deal with LODs.
	DestroyOctree(node);
}