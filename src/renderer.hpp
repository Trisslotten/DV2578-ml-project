#pragma once

#include "window.hpp"
#include "shader.hpp"
#include "camera.hpp"

class Renderer
{
	GLuint quadVAO;
	GLuint quadVBO;

	ShaderProgram shader;

	Camera camera;

	Timer timer;
	Timer dt;

public:
	void init();

	void render();
};