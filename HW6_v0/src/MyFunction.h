#pragma once
#ifndef MYFUNCTION
#define MYFUNCTION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GlobalVariables.h"

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
#endif // !MYFUNCTION
