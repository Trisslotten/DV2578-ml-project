#pragma once
#include <iostream>
#include <GL/glew.h>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <glm/glm.hpp>
class ComputeShader
{
private:
	std::string path;

	GLuint program;
	std::unordered_map<std::string, GLuint> uniform_locations;
	GLuint findUniformLocation(const std::string& name);
public:
	bool create(const std::string &path);
	GLuint getProgram()
	{
		return program;
	}
	void reload();

	void use();
	void uniformv(const std::string& name, GLuint count, const glm::mat4* matrices);
	void uniform(const std::string& name, const glm::mat4& matrix);

	void uniformv(const std::string& name, GLuint count, const GLfloat* values);
	void uniform(const std::string& name, const GLfloat value);

	void uniformv(const std::string& name, GLuint count, const glm::vec2* vectors);
	void uniform(const std::string& name, const glm::vec2& vector);

	void uniformv(const std::string& name, GLuint count, const glm::vec3* vectors);
	void uniform(const std::string& name, const glm::vec3& vector);

	void uniformv(const std::string& name, GLuint count, const glm::vec4* vectors);
	void uniform(const std::string& name, const glm::vec4& vector);

	void uniformv(const std::string& name, GLuint count, const GLint* values);
	void uniform(const std::string& name, const GLint value);
};