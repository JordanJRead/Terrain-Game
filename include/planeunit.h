#ifndef PLANE_UNIT_H
#define PLANE_UNIT_H

#include "planei.h"
#include <vector>
#include "glm/glm.hpp"

class PlaneUnit : public PlaneI {
public:
	PlaneUnit(int verticesPerEdge);
	const std::vector<float>& getVertexData() const { return mVertexData; }
	const std::vector<unsigned int>& getIndices() const { return mIndices; }

private:
	std::vector<unsigned int> mIndices;
	std::vector<float> mVertexData;
};

#endif