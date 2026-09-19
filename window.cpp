#include "window.h"
#include <stdio.h>
#include <iostream>

Window::Window() : Window(800, 600)
{
}

Window::Window(GLint windowWidth, GLint windowHeight)
    : mainWindow_(nullptr), width_(windowWidth), height_(windowHeight),
      bufferWidth_(0), bufferHeight_(0), lastX_(0.0f), lastY_(0.0f),
      xChange_(0.0f), yChange_(0.0f), mouseRightButtonPressed_(false),
      mouseFirstMoved_(true), keys_{}, leftClickPending_(false), clickX_(0.0), clickY_(0.0)
{
}

int Window::Initialise()
{
	// Initialise GLFW
	if (!glfwInit()) {
        std::cerr << "Ошибка инициализации GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

	// Setup GLFW Windows Properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	// Core Profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatiblity
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create the window
	mainWindow_ = glfwCreateWindow(width_, height_, "Test Window", NULL, NULL);
	if (!mainWindow_)
	{
		printf("Error creating GLFW window!");
		glfwTerminate();
		return 1;
	}

	// Get buffer size information
	glfwGetFramebufferSize(mainWindow_, &bufferWidth_, &bufferHeight_);

	// Set the current context
	glfwMakeContextCurrent(mainWindow_);

	// Allow modern extension access
	glewExperimental = GL_TRUE;

	// Handle key + mouse
	createCallbacks();
	// glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		printf("Error: %s", glewGetErrorString(error));
		glfwDestroyWindow(mainWindow_);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	// Create Viewport
	glViewport(0, 0, bufferWidth_, bufferHeight_);
	glfwSetWindowUserPointer(mainWindow_, this);

	return 0;
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow_, handleKeys);
	glfwSetMouseButtonCallback(mainWindow_, handleMouseButtons);
	glfwSetCursorPosCallback(mainWindow_, handleMouse);
}

void Window::handleKeys(GLFWwindow* window, int key, int c, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) theWindow->keys_[key] = true;
		else if (action == GLFW_RELEASE) theWindow->keys_[key] = false;
	}
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (theWindow->mouseRightButtonPressed_)
	{
		if (theWindow->mouseFirstMoved_)
		{
			theWindow->lastX_ = xPos;
			theWindow->lastY_ = yPos;
			theWindow->mouseFirstMoved_ = false;
		}

		theWindow->xChange_ += xPos - theWindow->lastX_;
		theWindow->yChange_ += theWindow->lastY_ - yPos;
		theWindow->lastX_ = xPos;
		theWindow->lastY_ = yPos;
	}
	else theWindow->mouseFirstMoved_ = true;

}

void Window::handleMouseButtons(GLFWwindow* window, int button, int action, int mods)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        theWindow->mouseRightButtonPressed_ = action == GLFW_PRESS;
        theWindow->mouseFirstMoved_ = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &theWindow->clickX_, &theWindow->clickY_);
        theWindow->leftClickPending_ = true;
    }
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow_);
	glfwTerminate();
}


bool Window::consumeLeftClick(double& x, double& y)
{
    if (!leftClickPending_) return false;
    leftClickPending_ = false;
    x = clickX_;
    y = clickY_;
    return true;
}
