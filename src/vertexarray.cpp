#include <vertexarray.h>

void VertexArray::create(const std::vector<float>& vertexData, const std::vector<unsigned int>& vertexIndices, const std::vector<int>& vertexAttribCounts) {
	mIndexCount = vertexIndices.size();
	mVAO.bind();

	mVBO.bind(GL_ARRAY_BUFFER);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &(vertexData[0]), GL_STATIC_DRAW);

	mEBO.bind(GL_ELEMENT_ARRAY_BUFFER);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(unsigned int), &(vertexIndices[0]), GL_STATIC_DRAW);

	int floatsPerVertex{ 0 };
	for (int attribCount : vertexAttribCounts)
		floatsPerVertex += attribCount;
	int bytesPerVertex{ static_cast<int>(floatsPerVertex * sizeof(float)) };

	int currentByteOffset{ 0 };
	for (int i{ 0 }; i < vertexAttribCounts.size(); ++i) {
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, vertexAttribCounts[i], GL_FLOAT, GL_FALSE, bytesPerVertex, (void*)((intptr_t)currentByteOffset));
		currentByteOffset += vertexAttribCounts[i] * sizeof(float);
	}
}

void VertexArray::updateVertices(const std::vector<float>& vertexData, const std::vector<unsigned int>& vertexIndices) {
	mIndexCount = vertexIndices.size();
	mVAO.bind();

	mVBO.bind(GL_ARRAY_BUFFER);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &(vertexData[0]), GL_STATIC_DRAW);

	mEBO.bind(GL_ELEMENT_ARRAY_BUFFER);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(unsigned int), &(vertexIndices[0]), GL_STATIC_DRAW);
}

VertexArray VertexArray::createScreenVertexArray() {
	std::vector<float> vertexData{
	-1, -1,
	 1, -1,
	-1,  1,
	 1,  1
	};

	std::vector<unsigned int> indices{
		0, 1, 2, 2, 1, 3
	};

	std::vector<int> attribs{
		2
	};

	VertexArray screen;
	screen.create(vertexData, indices, attribs);
	return screen;
}