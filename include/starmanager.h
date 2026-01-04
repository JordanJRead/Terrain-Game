#ifndef STAR_MANAGER_H
#define STAR_MANAGER_H

#include <array>
#include <glm/glm.hpp>
#include "constants.h"
#include "OpenGLObjects/BUF.h"

struct StarParameters {
	float minSize;
	float maxSize;
	int count;

	bool operator==(const StarParameters& other) const = default;
	StarParameters(float _minSize, float _maxSize, int _count)
		: minSize{ _minSize }
		, maxSize{ _maxSize }
		, count{ _count } { }
};

class StarManager {
public:
	StarManager(int bufferUnit);
	void update(const StarParameters& starParamters);

private:
	static int getSplitIndex(const glm::vec3& dir);
	StarParameters mPrevParameters;
	BUF mBuffer;
};

#endif