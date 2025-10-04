#ifndef APP_H
#define APP_H

#include "camera.h"
#include "shader.h"
#include "GLFW/glfw3.h"
#include "imgui_impl_glfw.h"
#include "terrainrenderer.h"

class App {
public:
	App(int screenWidth, int screenHeight, GLFWwindow* window);
	void loop();

private:
	int mScreenWidth;
	int mScreenHeight;
	Camera mCamera;
	TerrainRenderer mTerrainRenderer;
	GLFWwindow* mWindow;
	Shader mPhysicsShader{ "assets/shaders/physics.vert", "assets/shaders/physics.frag" };
	bool mIsCursorHidden{ true };

	static void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
		App& app{ *static_cast<App*>(glfwGetWindowUserPointer(window)) };
		app.mCamera.mouseCallback(window, xPos, yPos, app.mIsCursorHidden);
		ImGui_ImplGlfw_CursorPosCallback(window, xPos, yPos);
	}

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		App& app{ *static_cast<App*>(glfwGetWindowUserPointer(window)) };
		if (key == GLFW_KEY_R && action == GLFW_PRESS) {
			if (app.mIsCursorHidden)
				glfwSetInputMode(app.mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			else
				glfwSetInputMode(app.mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			app.mIsCursorHidden = !app.mIsCursorHidden;
		}

		if (key == GLFW_KEY_H && action == GLFW_PRESS) {
			app.mTerrainRenderer.toggleUI();
		}
	}
	void handleInput();
};

#endif