#include "window.hpp"

#include <algorithm>
#include <stdlib.h>
#include <iostream>
#include <unordered_map>
#include <glm/glm.hpp>
#include <string>

namespace
{
	std::unordered_map<GLFWwindow*, glm::vec2 > scrolls;

	void scrollCallback(GLFWwindow* window, double x, double y)
	{
		scrolls[window] += glm::vec2(x, y);
	}

	void resizeCallback(GLFWwindow* window, int width, int height)
	{
		//height = width * 9.0 / 16.0;
		//glfwSetWindowSize(window, width, height);
		glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	}


	GLFWwindow* window = nullptr;
	glm::vec2 scroll;

	Timer frame_timer;
	std::vector<double> times;
	double accum_frame_time;
	double median;
	double max;
	double frames;

	std::string title;
	glm::vec2 last_mouse_pos;
	glm::dvec2 mouse_pos;

	void pollEvents()
	{
		glfwPollEvents();
	}
	void swapBuffers()
	{
		glfwSwapBuffers(window);
	}
	void updateMousepos()
	{
		last_mouse_pos = mouse_pos;
		glfwGetCursorPos(window, &mouse_pos.x, &mouse_pos.y);
	}
	void updateScrolls()
	{
		scroll = scrolls[window];
		scrolls[window] = glm::vec2(0, 0);
	}
	void updateTitle()
	{
		/*
		double elapsed = frame_timer.restart();
		times.push_back(elapsed);
		accum_frame_time += elapsed;
		*/
		double update_time = 1.0;


		//if (accum_frame_time >= update_time)
		if (frame_timer.elapsed() >= update_time)
		{

			double elapsed = frame_timer.restart();
			double fps = frames / elapsed;
			std::string new_title = title + " | fps: " + std::to_string(fps);

			/*
			accum_frame_time -= update_time;
			std::sort(times.begin(), times.end());
			if (!times.empty())
			{
				int middle = times.size() / 2;
				median = times[middle];
				max = times[times.size()-1];
			}
			std::string new_title = title + " | median ms: " + std::to_string(median*1000.0) + " | max ms: " + std::to_string(max*1000.0);
			times.clear();
			*/

			glfwSetWindowTitle(window, new_title.c_str());
			frames = 0;
		}
		frames += 1.0;

	}
}



bool Window::isInitialized()
{
	if (window)
		return true;
	else
		return false;
}
void Window::open(int width, int height)
{
	if (window)
		return;

	if (!glfwInit())
		exit(EXIT_FAILURE);


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// MSAA
	glfwWindowHint(GLFW_SAMPLES, 8);

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!window)
	{
		std::cout << "ERROR: Could not create glfw window\n";
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "glewInit failed: " << std::string((const char*)glewGetErrorString(err)) << "\n";
		std::cin.ignore();
		exit(1);
	}

	scrolls[window] = glm::vec2(0, 0);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetWindowSizeCallback(window, resizeCallback);

	// vsync 1 off 0
	glfwSwapInterval(1);

	int major, minor, rev;
	glfwGetVersion(&major, &minor, &rev);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

	//glfwMaximizeWindow(window);
}

void Window::showCursor(bool val)
{
	if (val)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::close()
{
	glfwTerminate();
}

GLFWwindow * Window::getGLFWWindow()
{
	return window;
}

void Window::update()
{
	updateTitle();

	updateScrolls();

	updateMousepos();

	swapBuffers();

	pollEvents();
}

bool Window::shouldClose()
{
	if (!window)
		return true;
	return glfwWindowShouldClose(window);
}

bool Window::inFocus()
{
	return glfwGetWindowAttrib(window, GLFW_FOCUSED);
}

bool Window::mouseButtonDown(int button)
{
	return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

bool Window::keyDown(int key)
{
	return GLFW_PRESS == glfwGetKey(window, key);
}

glm::vec2 Window::mouseMovement()
{
	glm::vec2 pos = mousePosition();
	glm::vec2 result = pos - last_mouse_pos;
	return result;
}

glm::vec2 Window::mousePosition()
{
	return glm::vec2(mouse_pos);
}

glm::vec2 Window::size()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	return glm::vec2(width, height);
}

void Window::setTitle(const std::string & _title)
{
	title = _title;
}
