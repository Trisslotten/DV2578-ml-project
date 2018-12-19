#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


struct Camera
{
	glm::vec3 position;
	glm::quat orientation;

	float fov = 70.f;

	float pitch;
	float yaw;

	void update(float dt);

	glm::vec3 getLookDir();

	glm::mat4 getTransform();
};