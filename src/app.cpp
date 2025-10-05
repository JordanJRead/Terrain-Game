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

App::App(int screenWidth, int screenHeight, GLFWwindow* window)
	: mCamera{ screenWidth, screenHeight, {0, 20, 0} } // x = 2883548 for farlands
	, mWindow{ window }
	, mScreenWidth{ screenWidth }
	, mScreenHeight{ screenHeight }
	, mTerrainRenderer{ screenWidth, screenHeight, mCamera.getPosition(), std::array<glm::vec2, 4> {glm::vec2{0}, glm::vec2{0}, glm::vec2{0}}, mUIManager }
{
	glfwSetWindowUserPointer(mWindow, this);
	glfwSetCursorPosCallback(mWindow, mouseCallback);
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
	glClearColor(0.5f, 0.5f, 0.5f, 1);
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

		// Physics
		PlanePhysics physicsPlane{ 20, mCamera.getPosition(), 10, mTerrainRenderer, mUIManager};

		/// Input
		handleInput();
		mCamera.move(mWindow, (float)deltaTime, physicsPlane);

		/// Rendering
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Terrain
		mTerrainRenderer.render(mCamera, (float)glfwGetTime(), mUIManager);
		if (mIsUIVisible)
			mUIManager.render(deltaTime);


		// Debug physics plane
		//PlaneGPU gpuPlane{ physicsPlane };
		//mPhysicsShader.use();
		//mPhysicsShader.setMatrix4("view", mCamera.getViewMatrix());
		//mPhysicsShader.setMatrix4("proj", mCamera.getProjectionMatrix());
		//gpuPlane.useVertexArray();
		//glDisable(GL_DEPTH_TEST);
		//glDrawElements(GL_TRIANGLES, gpuPlane.getIndexCount(), GL_UNSIGNED_INT, 0);
		//glEnable(GL_DEPTH_TEST);

		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}