#ifndef PLANE_PHYSICS_H
#define PLANE_PHYSICS_H

#include "planei.h"
#include "glm/glm.hpp"
#include <functional>
#include <vector>

class TerrainRenderer;

class PlanePhysics : public PlaneI {
public:
	PlanePhysics(int verticesPerEdge, const glm::vec3& worldPos, float width, const TerrainRenderer& terrainRenderer);
	glm::vec3 getClosestWorldVertexPos(const glm::vec3 pos);
	float getStepSize() const { return (1.0 / (mVerticesPerEdge - 1)) * mWidth; }
	const std::vector<float>& getVertexData() const { return mVertexData; }
	const std::vector<unsigned int>& getIndices() const { return mIndices; }

private:
	float mWidth;
	std::vector<unsigned int> mIndices;
	std::vector<float> mVertexData;
};

#endif