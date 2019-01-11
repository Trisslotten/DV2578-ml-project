#include "computeshader.hpp"

bool ComputeShader::create(const std::string & path)
{
	this->path = path;
	std::ifstream file(path);
	std::string c = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	const char *code = c.c_str();
	const GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
	if (shader)
	{
		glShaderSource(shader, 1, &code, NULL);
		glCompileShader(shader);
		const GLuint program = glCreateProgram();
		if (program)
		{
			GLint compiled = GL_FALSE;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
			glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);
			if (compiled)
			{
				glAttachShader(program, shader);
				glLinkProgram(program);
				glDetachShader(program, shader);
			}
			else
			{
				GLint success = 0;
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (success == GL_FALSE)
				{
					GLint log_size = 0;
					glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);
					std::vector<GLchar> error(log_size);
					glGetShaderInfoLog(shader, log_size, &log_size, &error[0]);
					std::string errorstr{ &error[0] };

					std::cout << "Error in '" << path << "'\n" << errorstr << "\n";

					glDeleteShader(shader);
					system("pause");
					return false;
				}
			}
		}
		glDeleteShader(shader);
		this->program = program;
		return true;
	}
	else
	{
		return false;
	}
}
GLuint ComputeShader::findUniformLocation(const std::string & name)
{
	auto it = uniform_locations.find(name);
	GLuint uniform_location;
	if (it == uniform_locations.end())
	{
		uniform_location = glGetUniformLocation(program, name.c_str());
		if (uniform_location == -1)
		{
			std::cout << "ERROR: could not find '" << name << "' in shader\n";
		}
		uniform_locations[name] = uniform_location;
	}
	else
		uniform_location = it->second;
	return uniform_location;
}



void ComputeShader::reload()
{
	glDeleteProgram(program);
	create(path);
}

void ComputeShader::use()
{
	glUseProgram(program);
}
void ComputeShader::uniformv(const std::string & name, GLuint count, const glm::mat4* matrices)
{
	glUniformMatrix4fv(findUniformLocation(name), count, GL_FALSE, (GLfloat*)matrices);
}
void ComputeShader::uniform(const std::string & name, const glm::mat4& matrix)
{
	uniformv(name, 1, &matrix);
}

void ComputeShader::uniformv(const std::string & name, GLuint count, const GLfloat* values)
{
	glUniform1fv(findUniformLocation(name), count, (GLfloat*)values);
}
void ComputeShader::uniform(const std::string & name, GLfloat value)
{
	uniformv(name, 1, &value);
}

void ComputeShader::uniformv(const std::string & name, GLuint count, const glm::vec2* vectors)
{
	glUniform2fv(findUniformLocation(name), count, (GLfloat*)vectors);
}
void ComputeShader::uniform(const std::string & name, const glm::vec2& vector)
{
	uniformv(name, 1, &vector);
}

void ComputeShader::uniformv(const std::string & name, GLuint count, const glm::vec3* vectors)
{
	glUniform3fv(findUniformLocation(name), count, (GLfloat*)vectors);
}
void ComputeShader::uniform(const std::string & name, const glm::vec3& vector)
{
	uniformv(name, 1, &vector);
}

void ComputeShader::uniformv(const std::string & name, GLuint count, const glm::vec4* vectors)
{
	glUniform4fv(findUniformLocation(name), count, (GLfloat*)vectors);
}
void ComputeShader::uniform(const std::string & name, const glm::vec4& vector)
{
	uniformv(name, 1, &vector);
}

void ComputeShader::uniformv(const std::string & name, GLuint count, const GLint* values)
{
	glUniform1iv(findUniformLocation(name), count, values);
}
void ComputeShader::uniform(const std::string & name, GLint value)
{
	uniformv(name, 1, &value);
}