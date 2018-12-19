#pragma once

#include <glm\glm.hpp>

namespace Input
{
	bool initialize();
	// key is GLFW keycode
	bool isKeyDown(int key);
	bool isKeyPressed(int key);
	// TODO implement if needed
	//bool isKeyPressedRepeat(int key);
	bool isMouseButtonDown(int button);
	bool isButtonPressed(int button);
	void reset();
	glm::vec2 mouseMovement();
	glm::vec2 mousePosition();

	bool isGamepadPresent();
	float gamepadAxis(int axis);
	bool gamepadButtonPressed(int button);
};


