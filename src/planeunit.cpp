#include "planeunit.h"

PlaneUnit::PlaneUnit(int verticesPerEdge)
	: PlaneI{ verticesPerEdge }
{
	constexpr int floatsPerVertex{ 2 };
	int vertexCount{ mVerticesPerEdge * mVerticesPerEdge };

	mVertexData.reserve(vertexCount * floatsPerVertex);

	double stepSize{ 1.0 / (mVerticesPerEdge - 1) };
	double currentXPos{ -0.5 };
	double currentZPos{ -0.5 };
	for (int vertexI{ 0 }; vertexI < vertexCount; ++vertexI) {
		mVertexData.push_back(currentXPos);
		mVertexData.push_back(currentZPos);

		if ((vertexI + 1) % mVerticesPerEdge == 0) {
			currentXPos = -0.5;
			currentZPos += stepSize;
		}
		else {
			currentXPos += stepSize;
		}
	}

	int numOfQuads{ (mVerticesPerEdge - 1) * (mVerticesPerEdge - 1) };
	int numOfTriangles{ numOfQuads * 2 };
	mIndexCount = numOfTriangles * 3;
	mIndices.reserve(mIndexCount);

	int quadsFound{ 0 };
	for (int vertexI{ 0 }; vertexI < vertexCount; ++vertexI) {
		// Ignore vertices on right or top edge
		if ((vertexI + 1) % mVerticesPerEdge == 0 || vertexI >= vertexCount - mVerticesPerEdge)
			continue;

		mIndices.push_back(vertexI);
		mIndices.push_back(vertexI + mVerticesPerEdge);
		mIndices.push_back(vertexI + 1);

		mIndices.push_back(vertexI + 1);
		mIndices.push_back(vertexI + mVerticesPerEdge);
		mIndices.push_back(vertexI + 1 + mVerticesPerEdge);

		++quadsFound;
	}
}