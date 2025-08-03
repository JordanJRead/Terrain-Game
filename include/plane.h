#ifndef PLANE_H
#define PLANE_H

#include "glad/glad.h"
#include "glm/glm.hpp"
#include <vector>

class Plane {
public:
	Plane(int verticesPerEdge);
	int getIndexCount() const { return mIndexCount; }
	int getVerticesPerEdge() const { return mVerticesPerEdge; }
	float getStepSize() const { return 1.0 / (mVerticesPerEdge - 1); }
	const static int mAttribs{ 2 };
	const std::vector<float>& getVertices() { return mVertices; }
	const std::vector<unsigned int>& getIndices() { return mIndices; }

private:
	int mVerticesPerEdge;
	int mIndexCount;
	std::vector<float> mVertices;
	std::vector<unsigned int> mIndices;
};

#endif