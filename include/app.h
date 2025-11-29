#ifndef APP_H
#define APP_H

#include "cameraplayer.h"
#include "shader.h"
#include "GLFW/glfw3.h"
#include "imgui_impl_glfw.h"
#include "terrainrenderer.h"
#include "uimanager.h"
#include "framebuffer.h"
#include "vertexarray.h"

class App {
public:
	App(int screenWidth, int screenHeight, GLFWwindow* window);
	void loop();

private:
	int mScreenWidth;
	int mScreenHeight;
	bool mIsUIVisible{ true };
	CameraPlayer mCamera;
	UIManager mUIManager;
	TerrainRenderer mTerrainRenderer;
	GLFWwindow* mWindow;
	Shader mPhysicsShader{ "assets/shaders/physics.vert", "assets/shaders/physics.frag" };
	Shader mGammaShader{ "assets/shaders/gamma.vert", "assets/shaders/gamma.frag" };
	Framebuffer<1> mFramebuffer;
	VertexArray mScreenQuad;
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
			app.mIsUIVisible = !app.mIsUIVisible;
		}

		if (key == GLFW_KEY_F && action == GLFW_PRESS) {
			app.mCamera.toggleFreecam();
		}
	}
	void handleInput();
};

#endif