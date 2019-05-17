#include "Shader.h"
#include <fstream>
#include <sstream>
#include <string>

Shader::Shader(const char* vertexCodePath, const char* fragmentCodePath)
{
	string vertexCode, fragmentCode;
	ifstream vShaderFile, fShaderFile;
	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try {
		// open files
		vShaderFile.open(vertexCodePath);
		fShaderFile.open(fragmentCodePath);
		// read file's buffer into streams
		stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close files
		vShaderFile.close();
		fShaderFile.close();
		// convert to string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (ifstream::failure e) {
		cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << endl;
	}
	const char* vertexShaderCode;
 	const char* fragmentShaderCode;
	// 1. 获取顶点着色器的代码
	vertexShaderCode = vertexCode.c_str();
	// 2. 编译顶点着色器（创建一个着色器对象，然后将源码附加到着色器对象上，最后编译着色器对象）
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
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
	// 4. 获取片段着色器
	fragmentShaderCode = fragmentCode.c_str();
	// 5. 编译片段着色器
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
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
