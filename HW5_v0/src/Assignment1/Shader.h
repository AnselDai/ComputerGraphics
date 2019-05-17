#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <iostream>

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
};

#endif // !SHADER_H
