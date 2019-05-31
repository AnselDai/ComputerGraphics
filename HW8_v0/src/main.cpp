#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GlobalVariables.h"
#include "MyFunction.h"

using namespace std;

int main() {
	GLFWwindow* window;
	bool state;
	state = init_GLFW_window(window);
	if (!state) return -1;
	init_imgui(window);
	
	Shader MyShader = Shader("./points.vs", "./points.fs");

	glEnable(GL_DEPTH_TEST);
	// 处理输入
	process_input(window);
	// 处理鼠标
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		MyShader.use();
		MyShader.setVec3("MyColor", glm::vec3(255, 0, 0));
		render_controlPoints();
		MyShader.setVec3("MyColor", glm::vec3(255, 97, 0));
		render_curve();
		MyShader.setVec3("MyColor", glm::vec3(0, 255, 0));
		render_middle_process();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &controlVAO);
	glDeleteBuffers(1, &controlVBO);
	glDeleteVertexArrays(1, &curveVAO);
	glDeleteBuffers(1, &curveVBO);
	glDeleteVertexArrays(1, &middleVAO);
	glDeleteBuffers(1, &middleVBO);
	glfwTerminate();
	return 0;
}