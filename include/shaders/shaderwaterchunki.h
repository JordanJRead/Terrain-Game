#ifndef SHADER_WATER_CHUNK_I
#define SHADER_WATER_CHUNK_I

#include "shaders/shaderi.h"
#include "glm/glm.hpp"

class Cubemap;
class TerrainRenderer;

class ShaderWaterChunkI : public ShaderI {
public:

	ShaderWaterChunkI(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {}
	virtual void setRenderData(const TerrainRenderer& terrainRenderer, const glm::vec3& planePos, float planeWidth, const Cubemap& skybox) const = 0;
};

#endif