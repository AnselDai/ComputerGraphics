#include "Shader.h"

namespace code {
	const char* vertexShaderCode = "#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aColor;\n"
		"out vec3 myColor;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
		"   myColor = aColor;"
		"}\0";
	const char* fragmentShaderCode = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec3 myColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(myColor, 1.0f);\n"
		"}\0";
}

Shader::Shader()
{
	// 1. 编写顶点着色器

	// 2. 编译顶点着色器（创建一个着色器对象，然后将源码附加到着色器对象上，最后编译着色器对象）
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &code::vertexShaderCode, NULL);
	glCompileShader(vertexShader);
	// 3. 检查编译是否成功
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR::VERTEXSHADER::COMPILE" << endl << infoLog << endl;
		return;
	}
	// 4. 编写片段着色器

	// 5. 编译片段着色器
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &code::fragmentShaderCode, NULL);
	glCompileShader(fragmentShader);
	// 6. 检查编译是否成功
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cout << "ERROR::FRAGMENT::COMPILE" << endl << infoLog << endl;
		return;
	}
	// 6. 将两个着色器附加到一个着色器程序上
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	// 7. 检查链接是否失败
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		cout << "ERROR::PROGRAM::LINK" << endl << infoLog << endl;
		return;
	}
	// 8. 删除两个着色器对象
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return;
}

void Shader::use()
{
	glUseProgram(ID);
}
