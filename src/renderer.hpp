#pragma once

#include <vector>
#include "window.hpp"
#include "shader.hpp"
#include "camera.hpp"

const int WINDOW_SIZE_X = 128 * 6;
const int WINDOW_SIZE_Y = 128 * 6;

class Renderer
{
	GLuint quadVAO;
	GLuint quadVBO;

	GLuint normalDepthTextures[2];
	GLuint accumTextures[2];
	GLuint accumFramebuffers[2];
	int bufferIndex = 0;

	
	std::vector<float> imgBuffer;
	std::vector<float> ndBuffer;
	std::vector<float> dataBuffer;

	float numFrames = 0.f;
	const float numPasses = 128.f;
	float simTime = 0.f;

	glm::vec3 modulation = glm::vec3(1);
	const int numSame = 8;
	int sameCounter = 8;

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