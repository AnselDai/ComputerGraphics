#pragma once
#include <stb_image.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GlobalVariables.h"
#include "Shader.h"

#ifndef MYFUNCTION
#define MYFUNCTION

bool is_press(GLFWwindow* window, int KeyCode) {
	return glfwGetKey(window, KeyCode) == GLFW_PRESS;
}
// 初始化窗口
bool init_GLFW_window(GLFWwindow*& window) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__
	window = glfwCreateWindow(width, height, "Learning", NULL, NULL);
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
// 初始化gui
void init_imgui(GLFWwindow* window) {
#if __APPLE__
	const char* version = "#version 150";
#else
	const char* version = "#version 130";
#endif
	// 设置ImGui的上下文
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// 设置ImGui的样式
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(version);
}
// 处理输入
void process_input(GLFWwindow* window) {
	if (is_press(window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
	}
	if (is_press(window, GLFW_KEY_UP)) {
		camera.moveForward();
	}
	if (is_press(window, GLFW_KEY_DOWN)) {
		camera.moveBack();
	}
	if (is_press(window, GLFW_KEY_RIGHT)) {
		camera.moveRight();
	}
	if (is_press(window, GLFW_KEY_LEFT)) {
		camera.moveLeft();
	}
}

// 处理鼠标事件
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	camera.moveMouse(xpos, ypos);
}
// 处理滚轮事件
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.scroll(xoffset, yoffset);
}

unsigned int load_texture(const char* path) {
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	return texture;
}

void render_plane() {
	if (planeVAO == 0) {
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindVertexArray(0);
	}
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void render_cube() {
	if (cubeVAO == 0) {
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindVertexArray(0);
	}
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void render_quad() {
	if (quadVAO == 0) {
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void render_scene(const Shader& shader) {
	// plane
	glm::mat4 model = glm::mat4(1.0f);
	shader.setMat4("model", model);
	render_plane();
	// cube
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
	shader.setMat4("model", model);
	render_cube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0f));
	shader.setMat4("model", model);
	render_cube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0f));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	render_cube();
}

#endif // !MYFUNCTION
