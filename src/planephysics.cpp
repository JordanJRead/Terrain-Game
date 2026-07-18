#include "planephysics.h"
#include "planeunit.h"
#include "heightfunction.h"
#include "mathhelper.h"
#include "commonbuffertypes.h"

PlanePhysics::PlanePhysics(int verticesPerEdge, const glm::vec3& worldPos, float width, const CommonBufferTypes::TerrainParams& terrainParams, float terrainScale)
	: PlaneI{ verticesPerEdge }
	, mWidth{ width }
{
	constexpr int floatsPerVertex{ 3 };

	PlaneUnit basePlane{ verticesPerEdge };
	mIndices = basePlane.getIndices();
	mIndexCount = basePlane.getIndexCount();
	int vertexCount{ mVerticesPerEdge * mVerticesPerEdge };

	mVertexData.reserve(vertexCount * floatsPerVertex);

	const std::vector<float>& baseVertexData{ basePlane.getVertexData() };
	glm::vec3 flatStepWorldPos{ getClosestWorldVertexPos(worldPos) };

	for (int baseVertexI{ 0 }; baseVertexI < baseVertexData.size() - 1; baseVertexI += 2) {
		float xValue{ baseVertexData[baseVertexI] };
		float zValue{ baseVertexData[baseVertexI + 1] };
		xValue *= mWidth;
		xValue += flatStepWorldPos.x;
		zValue *= mWidth;
		zValue += flatStepWorldPos.z;
		float yValue = HeightFunction::getHeightAtPoint({ xValue, zValue }, terrainParams, terrainScale);
		mVertexData.push_back(xValue);
		mVertexData.push_back(yValue);
		mVertexData.push_back(zValue);
	}
}

glm::vec3 PlanePhysics::getClosestWorldVertexPos(const glm::vec3 pos) {
	return MathHelper::getClosestWorldStepPosition(pos, getStepSize());
}