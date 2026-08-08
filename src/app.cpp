#include "app.h"
#include <vector>
#include "vertexarray.h"
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <array>
#include "glm/glm.hpp"
#include "planephysics.h"
#include "planegpu.h"
#include "openglbuffer.h"

App::App(int screenWidth, int screenHeight, GLFWwindow* window)
	: mCamera{ screenWidth, screenHeight, {0, 20, 0} } // x = 2883548 for farlands
	, mWindow{ window }
	, mScreenWidth{ screenWidth }
	, mScreenHeight{ screenHeight }
	, mTerrainRenderer{ screenWidth, screenHeight, mCamera.getPosition() }
	, mFramebuffer{ 1, screenWidth, screenHeight, GL_RGBA32F }
{
	glfwSetWindowUserPointer(mWindow, this);
	glfwSetCursorPosCallback(mWindow, mousePositionCallback);
	glfwSetMouseButtonCallback(mWindow, mouseClickCallback);
	glfwSetKeyCallback(mWindow, keyCallback);
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_MULTISAMPLE);
	glViewport(0, 0, screenWidth, screenHeight);
	glPointSize(5);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);
}

void App::handleInput() {
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(mWindow, 1);
	}
	if (glfwGetKey(mWindow, GLFW_KEY_R) == GLFW_PRESS) {
		int x = 0;
	}
}

void App::loop() {
	double deltaTime{ 0 };
	double prevFrame{ glfwGetTime() };

	bool wireModeGUI{ false };
	bool displayGridGUI{ true };

	glfwSwapInterval(0);

	double startTime{ glfwGetTime() };

	while (!glfwWindowShouldClose(mWindow)) {

		deltaTime = glfwGetTime() - prevFrame;
		prevFrame = glfwGetTime();
		//-385 1703 - cool thing
		// Physics
		PlanePhysics physicsPlane{ 30, mCamera.getPosition(), 10, mTerrainRenderer.getTerrainParams(), mTerrainRenderer.getTerrainScale() };

		/// Input
		handleInput();
		mCamera.move(mWindow, (float)deltaTime, physicsPlane);

		/// Rendering
		mFramebuffer.clear({0.5f, 0.5f, 0.5f, 1}, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// UI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		mTerrainRenderer.updateAndRenderUI(mCamera.getPosition(), mIsUIVisible);

		if (mIsUIVisible) {
			mFPSFrameCount++;
			ImGui::Begin("Physics plane");
			ImGui::Checkbox("Show", &mShowPhysicsPlane);
			ImGui::End();

			ImGui::Begin("FPS");
			ImGui::Text(std::to_string((int)mDisplayFPS).c_str());
			ImGui::End();

			if (mFPSUpdateInterval.update(deltaTime)) {
				mDisplayFPS = mFPSFrameCount / mFPSUpdateInterval.getPeriod();
				mFPSFrameCount = 0;
			}
		}
		else {
			mFPSFrameCount = 0;
		}

		// Terrain
		mTerrainRenderer.render(mCamera, (float)glfwGetTime(), mFramebuffer, mDebugFragPos);
		if (mDebugFragPos != glm::ivec2{-1, -1}) {
			mDebugFragPos = glm::ivec2{ -1, -1 };
		}

		// Gamma
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, mScreenWidth, mScreenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		mGammaShader.render(nullptr, mScreenQuad, mFramebuffer.getColourTex(0), mTerrainRenderer.getDeferredRenderer().getNoise());

		// Debug physics plane
		if (mShowPhysicsPlane) {
			PlaneGPU gpuPlane{ physicsPlane };
			mPhysicsShader.render(nullptr, gpuPlane);
		}

		//auto debugData{ mTerrainRenderer.getDebugData() };
		//if (debugData.hasData) {
		//	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Debug Line");
		//	mLineShader.render(nullptr, debugData.worldStart, debugData.worldEnd, InputSpace::World, { 0, 0, 1 }, { 1, 0, 0 });
		//	int w, h;
		//	glfwGetFramebufferSize(mWindow, &w, &h);

		//	for (size_t i{ 0 }; i < debugData.currentPointCount; ++i) {
		//		float t{ debugData.pointLine[i].w };
		//		mCircleShader.render(nullptr, debugData.pointLine[i], 0.01f, InputSpace::World, t * glm::vec3{0, 0, 1}, (float)w / h);
		//		mCircleShader.render(nullptr, debugData.pointScene[i], 0.01f, InputSpace::World, t * glm::vec3{ 0, 1, 0 }, (float)w / h);
		//		mCircleShader.render(nullptr, glm::vec4{ debugData.pointUV[i], 0, 1 }, 0.01f, InputSpace::UV, t * glm::vec3{ 1, 1, 0 }, (float)w / h);
		//	}
		//	mCircleShader.render(nullptr, debugData.worldEnd, 0.013f, InputSpace::World, { 1, 1, 1 }, (float)w / h);
		//	glPopDebugGroup();
		//}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}