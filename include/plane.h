#ifndef PLANE_H
#define PLANE_H

#include "glad/glad.h"
#include "glm/glm.hpp"
#include <vector>
#include <functional>

class Plane {
public:
	Plane(int verticesPerEdge);
	Plane(const Plane& basePlane, const glm::vec3 worldPos, float scale, std::function<float(const glm::vec2&)> heightFunction);
	int getIndexCount() const { return mIndexCount; }
	int getVerticesPerEdge() const { return mVerticesPerEdge; }
	float getStepSize() const { return 1.0 / (mVerticesPerEdge - 1); }
	int getFloatsPerVertex() const { return mFloatsPerVertex; }
	const std::vector<float>& getVertices() const { return mVertices; }
	const std::vector<unsigned int>& getIndices() const { return mIndices; }

private:
	int mVerticesPerEdge;
	int mIndexCount;
	int mFloatsPerVertex;
	std::vector<float> mVertices;
	std::vector<unsigned int> mIndices;
};

#endif