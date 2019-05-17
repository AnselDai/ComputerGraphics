#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define PI 3.1415926

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include "shader.h"

#include <vector>
#include <iostream>
using namespace std;

GLint statcky = 60;	// 横向向切成多少片
GLint stlicex = 60; // 纵向切多少片

const int width = 800, height = 800;

bool init_GLFW_window(GLFWwindow*& window);
void configure_vertex_attributes(unsigned int& VBO, unsigned int& VAO, unsigned int& EBO, vector<float>& vertices, vector<int>& indices) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), &indices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

vector<float> vertices;
vector<int> indices;


void set_sphere_vertices(float radius, vector<float>& vertices, vector<int>& indices) {
	GLfloat R = radius;	// 半径
	GLfloat angleHy = (GLfloat)(2 * PI) / statcky;	// 横向每份的角度		算出弧度值
	GLfloat angleZx = (GLfloat)(2 * PI) / stlicex;	// 纵向每份的角度		算出弧度值
	GLfloat NumAngleHy = 0;	// 当前横向角度
	GLfloat NumAngleZx = 0;	// 当前纵向角度

	GLfloat x = 0;
	GLfloat y = 0;
	GLfloat z = 0;
	for (float j = 0; j < statcky; j++)
	{
		for (float i = 0; i < stlicex; i++)
		{
			NumAngleHy = angleHy * i;
			NumAngleZx = angleZx * j; // 起点都是轴指向的方向。根据右手定则决定转向，只要转向相同，那么两个就合适
			GLfloat x = R * cos(NumAngleHy)*cos(NumAngleZx);	// 记得转化精度
			GLfloat y = R * sin(NumAngleHy);
			GLfloat z = R * cos(NumAngleHy)*sin(NumAngleZx);
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
	}
	int only = vertices.size();
	int num = (int)((only / (3 * statcky)) * 2);
	for (int x = 0; x < stlicex / 2;)
	{
		for (int y = 0; y < statcky; y++)
		{
			indices.push_back(y + x * stlicex);
			indices.push_back(y + x * stlicex + 1);
			indices.push_back(y + x * stlicex + stlicex);
			indices.push_back(y + x * stlicex + stlicex + 1);
			indices.push_back(y + x * stlicex + stlicex);
			indices.push_back(y + x * stlicex + 1);
		}
		x = x + 1;
	}
}

int main()
{
	GLFWwindow* window;
	bool state;
	state = init_GLFW_window(window);
	if (!state) return -1;
	glEnable(GL_DEPTH_TEST);
	Shader lampShader = Shader();
	glm::vec3 translations[2];
	translations[0] = glm::vec3(0.0f, 0.0f, 0.0f);
	translations[1] = glm::vec3(-1.0f, 0.0f, 0.0f);
	glm::vec3 colors[2];
	colors[0] = glm::vec3(1.0f, 0.0f, 0.0f);
	colors[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	unsigned int instanceVBO, colorVBO;
	// position
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 2, &translations[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// color
	glGenBuffers(1, &colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 2, &colors[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	set_sphere_vertices(0.2f, vertices, indices);
	unsigned int VBO, cubeVAO, EBO;
	// VAO
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(cubeVAO);
	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), &indices[0], GL_STATIC_DRAW);
	// color
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(1, 1);
	// position
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(2, 1);
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		lampShader.use();

		glm::mat4 model = glm::mat4(1.0f);
		// model = glm::rotate(model, glm::radians((float)glfwGetTime() * 5), glm::vec3(1.0f, 0.3f, 0.5f));
		lampShader.setMat4("transform", model);
		glm::mat4 sizeScale[2];
		glm::mat4 temp = glm::mat4(1.0f);
		sizeScale[0] = glm::scale(temp, glm::vec3(1.0f, 1.0f, 1.0f));
		sizeScale[1] = glm::scale(temp, glm::vec3(0.5f, 0.5f, 0.5f));
		glm::mat4 posMatrix[2];
		glm::mat4 befor[2], after[2];
		posMatrix[0] = glm::rotate(temp, (float)glfwGetTime()*glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 1.0f));
		//posMatrix[1] = glm::translate(temp, glm::vec3(0.8f, 0.0f, 0.0f));
		posMatrix[1] = glm::rotate(temp, (float)glfwGetTime()*glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 1.0f));
		//posMatrix[1] = glm::translate(posMatrix[1], glm::vec3(-0.8f, 0.0f, 0.0f));
		for (int i = 0; i < 2; i++) {
			stringstream ss;
			string index;
			ss << i;
			index = ss.str();
			lampShader.setMat4(("sizeScale[" + index + "]").c_str(), sizeScale[i]);
			lampShader.setMat4(("posMatrix[" + index + "]").c_str(), posMatrix[i]);
		}
		glBindVertexArray(cubeVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, indices.size(), 2);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

bool init_GLFW_window(GLFWwindow*& window) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__
	window = glfwCreateWindow(width, height, "Homework 2", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return false;
	}
	return true;
}
