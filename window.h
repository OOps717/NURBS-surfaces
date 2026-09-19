#pragma once

#include "stdio.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window();

	Window(GLint windowWidth, GLint windowHeight);

	int Initialise();

	GLint getBufferWidth() { return bufferWidth_; }
	GLint getBufferHeight() { return bufferHeight_; }

	bool getShouldClose() { return glfwWindowShouldClose(mainWindow_); }
	void swapBuffers() { glfwSwapBuffers(mainWindow_); }

	bool* getKeys() { return keys_; }
	GLfloat getXChange() {
		GLfloat x = xChange_;
		xChange_ = 0;
		return x;
	}
	GLfloat getYChange() {
		GLfloat y = yChange_;
		yChange_ = 0;
		return y;
	}

	bool isLeftMousePressed() const { return glfwGetMouseButton(mainWindow_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS; }
	void getCursorPosition(double& x, double& y) const { glfwGetCursorPos(mainWindow_, &x, &y); }
	bool consumeLeftClick(double& x, double& y);
	void getWindowSize(int& width, int& height) const { glfwGetWindowSize(mainWindow_, &width, &height); }
	void refreshBufferSize() { glfwGetFramebufferSize(mainWindow_, &bufferWidth_, &bufferHeight_); }

	~Window();

private:
	GLFWwindow* mainWindow_;

	GLint width_, height_;
	GLint bufferWidth_, bufferHeight_;
	GLfloat lastX_, lastY_, xChange_, yChange_;

	bool mouseRightButtonPressed_;
	bool mouseFirstMoved_;
	bool keys_ [1024];
	bool leftClickPending_;
	double clickX_, clickY_;

	void createCallbacks();
	static void handleKeys(GLFWwindow* window, int key, int c, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
	static void handleMouseButtons(GLFWwindow* window, int button, int action, int mods);
};