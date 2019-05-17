#include "Shader.h"

namespace code {
	const char* vertexShaderCode = "#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aColor;\n"
		"layout(location = 2) in vec3 aOffset;\n"
		"out vec3 myColor;\n"
		"uniform mat4 transform;\n"
		"uniform mat4 posMatrix[2];\n"
		"uniform mat4 sizeScale[2];\n"
		"void main()\n"
		"{\n"
		"   vec4 temp = vec4(aPos + aOffset, 1.0f) * sizeScale[gl_InstanceID];\n"
		"   gl_Position = transform * posMatrix[gl_InstanceID] * temp;\n"
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
	// 1. ��д������ɫ��

	// 2. ���붥����ɫ��������һ����ɫ������Ȼ��Դ�븽�ӵ���ɫ�������ϣ���������ɫ������
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &code::vertexShaderCode, NULL);
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
	// 4. ��дƬ����ɫ��

	// 5. ����Ƭ����ɫ��
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &code::fragmentShaderCode, NULL);
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
