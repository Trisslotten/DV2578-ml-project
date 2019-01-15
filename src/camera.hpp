#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


struct Camera
{
	glm::vec3 position = glm::vec3(0, 0, -7);
	glm::quat orientation = glm::quat();

	float fov = 75.f;
	float near = 0.5f;
	float far = 100.f;

	float pitch;
	float yaw;

	void update(float dt);

	glm::vec3 getLookDir();

	glm::mat4 getTransform();
};
