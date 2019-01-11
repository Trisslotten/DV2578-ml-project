#pragma once

#include <vector>
#include "window.hpp"
#include "shader.hpp"
#include "camera.hpp"

const int WINDOW_SIZE_X = 1024;
const int WINDOW_SIZE_Y = 1024;

class Renderer
{
	GLuint quadVAO;
	GLuint quadVBO;

	GLuint accumTextures[2];
	GLuint accumFramebuffers[2];
	int bufferIndex = 0;

	
	std::vector<float> imgBuffer;

	float numFrames = 0.f;
	const float numPasses = 100.f;
	float simTime = 0.f;


	int smoothGUID = 0;
	int noisyGUID = 0;

	ShaderProgram shader;
	ShaderProgram textureShader;

	Camera camera;

	Timer timer;
	Timer dt;

public:
	void init();

	void render();
};