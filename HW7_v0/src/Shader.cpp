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
	// 1. ��ȡ������ɫ���Ĵ���
	vertexShaderCode = vertexCode.c_str();
	// 2. ���붥����ɫ��������һ����ɫ������Ȼ��Դ�븽�ӵ���ɫ�������ϣ���������ɫ������
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
	glCompileShader(vertexShader);
	// 3. �������Ƿ�ɹ�
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR::VERTEXSHADER::COMPILE" << endl << infoLog << endl;
		return;
	}
	// 4. ��ȡƬ����ɫ��
	fragmentShaderCode = fragmentCode.c_str();
	// 5. ����Ƭ����ɫ��
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragmentShader);
	// 6. �������Ƿ�ɹ�
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cout << "ERROR::FRAGMENT::COMPILE" << endl << infoLog << endl;
		return;
	}
	// 6. ��������ɫ�����ӵ�һ����ɫ��������
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	// 7. ��������Ƿ�ʧ��
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		cout << "ERROR::PROGRAM::LINK" << endl << infoLog << endl;
		return;
	}
	// 8. ɾ��������ɫ������
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return;
}

void Shader::use()
{
	glUseProgram(ID);
}
