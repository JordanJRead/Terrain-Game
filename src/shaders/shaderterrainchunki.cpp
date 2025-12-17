#include "shaders/shaderterrainchunki.h"
#include "imagecount.h"

ShaderTerrainChunkI::ShaderTerrainChunkI(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {
	use();
	int i{ 0 };
	for (; i < ImageCount; ++i) {
		std::string indexString{ std::to_string(i) };
		setInt("images[" + indexString + "]", i);
	}
}