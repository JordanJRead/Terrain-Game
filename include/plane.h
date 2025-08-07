#ifndef PLANE_H
#define PLANE_H

#include "glad/glad.h"
#include "glm/glm.hpp"
#include <vector>
#include <functional>

class Plane {
public:
	Plane(int verticesPerEdge);

	// Creates a plane with XYZ coordinates from a base plane (width 1), using a function to determine the y value of each vertex
	Plane(const Plane& basePlane, const glm::vec3 worldPos, float scale, std::function<float(const glm::vec2&)> heightFunction);
	int getIndexCount() const { return mIndexCount; }
	int getVerticesPerEdge() const { return mVerticesPerEdge; }
	float getStepSize() const { return (1.0 / (mVerticesPerEdge - 1)) * mWidth; }
	int getFloatsPerVertex() const { return mFloatsPerVertex; }
	const std::vector<float>& getVertices() const { return mVertices; }
	const std::vector<unsigned int>& getIndices() const { return mIndices; }
	glm::vec3 getClosestWorldVertexPos(const glm::vec3 pos) {
		float stepSize{ getStepSize() };
		glm::vec3 stepSizesAway = pos / stepSize;
		stepSizesAway = glm::vec3{ (int)stepSizesAway.x, (int)stepSizesAway.y, (int)stepSizesAway.z };
		return stepSizesAway * stepSize;
	}

private:
	int mVerticesPerEdge;
	int mIndexCount;
	int mFloatsPerVertex;
	float mWidth;
	std::vector<float> mVertices;
	std::vector<unsigned int> mIndices;
};

#endif