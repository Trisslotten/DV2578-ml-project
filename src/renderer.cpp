#include "renderer.hpp"

#include <iostream>
#include <fstream>
#include "lodepng.h"


void Renderer::init()
{
	srand(time(nullptr));

	float quadVerts[] = {
	-1.f, 1.f, 0.f,
	-1.f, -1.f, 0.f,
	1.f, -1.f, 0.f,
	1.f, 1.f, 0.f,
	};
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glBindVertexArray(0);

	glDepthFunc(GL_ALWAYS);

	shader.add("shader.vert");
	shader.add("shader.frag");
	shader.compile();

	textureShader.add("drawtexture.vert");
	textureShader.add("drawtexture.frag");
	textureShader.compile();

	glClearColor(1.0, 0, 0, 1.0);

	
	

	glGenFramebuffers(2, accumFramebuffers);
	glGenTextures(2, accumTextures);
	glGenTextures(2, normalDepthTextures);
	for (int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, accumFramebuffers[i]);

		glBindTexture(GL_TEXTURE_2D, accumTextures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, GL_RGB, GL_FLOAT, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTextures[i], 0);

		glBindTexture(GL_TEXTURE_2D, normalDepthTextures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, GL_RGB, GL_FLOAT, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalDepthTextures[i], 0);

		GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR: Could not create framebuffer\n";
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	imgBuffer.resize(128 * 128 * 3);
	ndBuffer.resize(128 * 128 * 3);
	dataBuffer.resize(128 * 128 * 3 * 2);

	dt.restart();
}

void Renderer::render()
{
	camera.update(dt.restart());

	int next = (bufferIndex + 1) % 2;
	if (numFrames < numPasses)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, accumFramebuffers[bufferIndex]);
		shader.use();
		shader.uniform("viewProj", camera.getTransform());
		shader.uniform("cameraPos", camera.position);
		shader.uniform("time", float(timer.elapsed()));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, accumTextures[next]);
		shader.uniform("tex", 0);
		shader.uniform("numPasses", numPasses);
		shader.uniform("numFrames", numFrames);
		shader.uniform("simTime", simTime);
		shader.uniform("samplesPerPass", 128);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
	else
	{
		for (int y = 0; y < WINDOW_SIZE_Y / 128; y++)
		{
			for (int x = 0; x < WINDOW_SIZE_X / 128; x++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				int i = smoothGUID;
				smoothGUID++;
				int xOffset = x * 128;
				int yOffset = y * 128;
				glGetTextureSubImage(accumTextures[next], 0, xOffset, yOffset, 0, 128, 128, 1, GL_RGB, GL_FLOAT, sizeof(float) * 128 * 128 * 3, &imgBuffer[0]);
				std::ofstream file("data/img" + std::to_string(i) + ".bin", std::ios::out | std::ios::binary);
				file.write((char*)&imgBuffer[0], sizeof(float) * 128 * 128 * 3);
				file.close();
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, accumFramebuffers[bufferIndex]);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		shader.use();
		shader.uniform("viewProj", camera.getTransform());
		shader.uniform("cameraPos", camera.position);
		shader.uniform("time", float(timer.elapsed()));
		glBindTexture(GL_TEXTURE_2D, accumTextures[next]);
		shader.uniform("tex", 0);
		shader.uniform("numPasses", 1.f);
		shader.uniform("numFrames", 0.f);
		shader.uniform("simTime", simTime);
		shader.uniform("samplesPerPass", 4);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		for (int y = 0; y < WINDOW_SIZE_Y / 128; y++)
		{
			for (int x = 0; x < WINDOW_SIZE_X / 128; x++)
			{
				int i = noisyGUID;
				noisyGUID++;
				int xOffset = x * 128;
				int yOffset = y * 128;
				glGetTextureSubImage(accumTextures[bufferIndex], 0, xOffset, yOffset, 0, 128, 128, 1, GL_RGB, GL_FLOAT, sizeof(float) * 128 * 128 * 3, &imgBuffer[0]);
				glGetTextureSubImage(normalDepthTextures[bufferIndex], 0, xOffset, yOffset, 0, 128, 128, 1, GL_RGB, GL_FLOAT, sizeof(float) * 128 * 128 * 3, &ndBuffer[0]);
				for (int i = 0; i < 128 * 128; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						dataBuffer[6 * i + j] = imgBuffer[3 * i + j];
						dataBuffer[6 * i + j + 3] = ndBuffer[3 * i + j];
					}
				}
				std::ofstream file("data/noisy" + std::to_string(i) + ".bin", std::ios::out | std::ios::binary);
				file.write((char*)&dataBuffer[0], sizeof(float) * 128 * 128 * 3 * 2);
				file.close();
			}
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, accumFramebuffers[bufferIndex]);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, accumFramebuffers[next]);
		glClear(GL_COLOR_BUFFER_BIT);
		numFrames = 0.f;
		simTime = 100.f*rand() / float(RAND_MAX);
		camera.position.x = 6.f*rand() / float(RAND_MAX) - 3.f;
		camera.position.y = 6.f*rand() / float(RAND_MAX) - 3.f;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	textureShader.use();
	textureShader.uniform("tex", 0);
	textureShader.uniform("numPasses", numPasses);
	textureShader.uniform("numFrames", numFrames);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	bufferIndex = next;

	numFrames++;
}
