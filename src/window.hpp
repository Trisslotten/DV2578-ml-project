#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include "timer.hpp"

namespace Window
{
	bool isInitialized();
	void open(int width = 1280, int height = 720);
	void close();
	GLFWwindow *getGLFWWindow();

	// call each frame
	void update();

	bool shouldClose();
	bool inFocus();
	bool mouseButtonDown(int button);
	bool keyDown(int key);

	void showCursor(bool val);

	glm::vec2 mouseMovement();
	glm::vec2 mousePosition();
	glm::vec2 size();

	// set before open()
	void setTitle(const std::string& title);
};
