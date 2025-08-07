#include "vertexarray.h"
#include "glm/glm.hpp"
#include "plane.h"
#include "random.h"
#include <iostream>
#include <cassert>

Plane::Plane(int verticesPerEdge)
	: mVerticesPerEdge{ verticesPerEdge }
	, mFloatsPerVertex{ 2 }
{
	int vertexCount{ mVerticesPerEdge * mVerticesPerEdge };

	mVertices.reserve(vertexCount * mFloatsPerVertex);

	double stepSize{ 1.0 / (mVerticesPerEdge - 1) };
	double currentXPos{ -0.5 };
	double currentZPos{ -0.5 };
	for (int vertexI{ 0 }; vertexI < vertexCount; ++vertexI) {
		mVertices.push_back(currentXPos);
		mVertices.push_back(currentZPos);

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
		mIndices.push_back(vertexI + 1);
		mIndices.push_back(vertexI + mVerticesPerEdge);

		mIndices.push_back(vertexI + 1);
		mIndices.push_back(vertexI + mVerticesPerEdge);
		mIndices.push_back(vertexI + 1 + mVerticesPerEdge);

		++quadsFound;
	}
}

Plane::Plane(const Plane& basePlane, const glm::vec3 worldPos, float scale, std::function<float(const glm::vec2&)> heightFunction)
	: mVerticesPerEdge{ basePlane.getVerticesPerEdge() }
	, mIndexCount{ basePlane.getIndexCount() }
	, mFloatsPerVertex{ 3 }
	, mIndices{ basePlane.getIndices() }
{
	assert(basePlane.getFloatsPerVertex() == 2);

	int vertexCount{ mVerticesPerEdge * mVerticesPerEdge };

	mVertices.reserve(vertexCount * mFloatsPerVertex);

	const std::vector<float>& baseVertexData{ basePlane.getVertices() };
	glm::vec2 flatStepWorldPos;

	for (int baseVertexI{ 0 }; baseVertexI < baseVertexData.size() - 1; baseVertexI += 2) {
		float xValue{ baseVertexData[baseVertexI] };
		float zValue{ baseVertexData[baseVertexI + 1] };
		xValue *= scale; // preserve step size????
		xValue += flatStepWorldPos.x;
		zValue *= scale;
		zValue += flatStepWorldPos.y;
		float yValue = heightFunction({ xValue, zValue });
	}

	for (int baseVertexI{ 0 }; baseVertexI < baseVertexData.size(); ++baseVertexI) {
		bool isX{ baseVertexI % 2 == 0 };
		float value{ baseVertexData[baseVertexI] };
		value *= scale;
		value += isX ? flatStepWorldPos.x : flatStepWorldPos.y;
		if (isX) {
			float zValue{ baseVertexData[baseVertexI] };
			value *= scale;
			value += isX ? flatStepWorldPos.x : flatStepWorldPos.y;
			mVertices.push_back(value);
			mVertices.push_back(heightFunction({ baseVertexData[baseVertexI], baseVertexData[baseVertexI + 1] })); // add y
		}
		else { // z coord
			mVertices.push_back(baseVertexData[baseVertexI]);
		}
	}
}