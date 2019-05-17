#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <iostream>
#include <glm/glm.hpp>

using namespace std;

namespace code {
	extern const char* vertexShaderCode;
	extern const char* fragmentShaderCode;
}

class Shader {
public:
	unsigned int ID;
	Shader();
	void use();
	void setMat4(const string &name, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setFloat(const string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
};

#endif // !SHADER_H
