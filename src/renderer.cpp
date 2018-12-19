#include "renderer.hpp"




void Renderer::init()
{
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
	dt.restart();

	glClearColor(1.0, 0, 0, 1.0);
}

void Renderer::render()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera.update(dt.restart());

	shader.use();
	shader.uniform("viewProj", camera.getTransform());
	shader.uniform("time", float(timer.elapsed()));
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
