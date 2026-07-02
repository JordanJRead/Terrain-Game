#ifndef TERRAIN_RENDERER_H
#define TERRAIN_RENDERER_H

#include "glm/glm.hpp"
#include "vertexarray.h"
#include "planegpu.h"
#include "chunkmanager.h"
#include <array>
#include "cubemap.h"
#include "cubevertices.h"
#include "commonbuffertypes.h"
#include "deferredrenderer.h"
#include "constants.h"
#include "shadowmapper.h"
#include "shaders/shaderterrainimage.h"
#include "shaders/shaderchunk.h"
#include "shaders/shaderskybox.h"
#include "shaders/shaderortho.h"
#include "starmanager.h"
#include "openglbuffer.h"
#include "terrainimageset.h"

class FrameBufferI;
class FrameBufferColour;
class CameraI;
struct AABB;

class TerrainRenderer {
public:
	TerrainRenderer(int screenWidth, int screenHeight, const glm::vec3& cameraPos);

	void updateAndRenderUI(const glm::vec3& cameraPos);
	void render(const CameraPlayer& camera, float time, const FramebufferColour& targetFramebuffer);

	void bindTerrainImage(int i, int unit) const;
	const DeferredRenderer& getDeferredRenderer() const;
	const ShadowMapper<CascadeCount>& getShadowMapperSun() const;
	const ShadowMapper<CascadeCount>& getShadowMapperMoon() const;
	AABB getSceneWorldAABB(const glm::vec3& playerCameraPos) const;
	const CommonBufferTypes::TerrainParams& getTerrainParams() const { return mTerrainParams.mValue; }
	float getTerrainScale() const { return mArtisticParams.mValue.terrainScale; }

private:
	void renderTerrain(const FramebufferI& targetFramebuffer, const CameraI& camera, const glm::vec3& playerCameraPosition, ShaderChunk& terrainShader, ShaderChunk& waterShader, const glm::vec3& dirToSun, float time, bool depthPass = false, bool forceLowQuality = false);

private:
	OpenGLBuffer<CommonBufferTypes::TerrainParams>      mTerrainParams{ 0, BufferTypes::uniform, CommonBufferTypes::TerrainParams::getDefaultValue() };
	OpenGLBuffer<CommonBufferTypes::ArtisticParams>     mArtisticParams   { 1, BufferTypes::uniform, CommonBufferTypes::ArtisticParams::getDefaultValue() };
	OpenGLBuffer<CommonBufferTypes::WaterParams>        mWaterParams      { 2, BufferTypes::uniform, CommonBufferTypes::WaterParams::getDefaultValue() };
	OpenGLBuffer<CommonBufferTypes::ColourParams>       mColourParams     { 3, BufferTypes::uniform, CommonBufferTypes::ColourParams::getDefaultValue() };
	OpenGLBuffer<CommonBufferTypes::PerFrameInfo>       mPerFrameInfo     { 4, BufferTypes::uniform };
	OpenGLBuffer<CommonBufferTypes::TerrainImagesInfo>  mTerrainImagesInfo{ 5, BufferTypes::ssbo };
	OpenGLBuffer<CommonBufferTypes::AtmosphereInfo>     mAtmosphereInfo   { 6, BufferTypes::uniform, CommonBufferTypes::AtmosphereInfo::getDefaultValue() };
	OpenGLBuffer<CommonBufferTypes::ShadowInfo>         mShadowInfo       { 7, BufferTypes::ssbo, CommonBufferTypes::ShadowInfo::getDefaultValue() };
	StarManager mStarManager{ 9 };

	std::array<glm::vec2, ImageCount> mImageWorldPositions;
	float mMinTerrainHeight;
	float mMaxTerrainHeight;

	ShaderTerrainImage mShaderTerrainImage;
	ShaderChunk mShaderTerrainForward;
	ShaderChunk mShaderWaterForward;
	ShaderSkybox mSkyboxShader;
	ShaderOrtho mShaderOrtho{ "assets/shaders/ortho.vert", "assets/shaders/ortho.frag" };
	Cubemap mDaySkybox;
	Cubemap mNightSkybox;
	CubeVertices mCubeVertices;
	DeferredRenderer mDeferredRenderer;
	ShadowMapper<CascadeCount> mShadowMapperSun;
	ShadowMapper<CascadeCount> mShadowMapperMoon;
	ChunkManager mChunkManager{ 3, 8, 5000.0f, 205, mTerrainParams.mValue };

	VertexArray mScreenQuad;
	int mShellCount{ 30 };
	float mDayTime{};
	bool mDoDeferredRendering{ true };
	bool mDoFrustumCulling{ true };
	TerrainImageSet mTerrainImageSet;
};

#endif