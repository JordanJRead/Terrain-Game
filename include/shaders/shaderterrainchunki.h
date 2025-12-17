#ifndef SHADER_TERRAIN_CHUNK_I
#define SHADER_TERRAIN_CHUNK_I

#include "shaders/shaderi.h"
#include "glm/glm.hpp"

class TerrainRenderer;
class Cubemap;

class ShaderTerrainChunkI : public ShaderI {
public:
	ShaderTerrainChunkI(const std::string& vertPath, const std::string& fragPath);
	virtual void setRenderData(const TerrainRenderer& terrainRenderer, const glm::vec3& planePos, float planeWidth, int shellCount, const Cubemap& skybox) = 0;
};

#endif