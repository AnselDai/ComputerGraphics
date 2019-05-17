#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "Shader.h"
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

bool init_GLFW_window(GLFWwindow*& window);
void configure_vertex_attributes(unsigned int& VBO, unsigned int& VAO, unsigned int& EBO);
void process_input(GLFWwindow* window);
void init_imgui(GLFWwindow* window);

float vertices[] = {
// 后面
	-2.0f, -2.0f, -2.0f,  1.0f, 0.0f, 0.0f,
	 2.0f, -2.0f, -2.0f,  1.0f, 0.0f, 0.0f,
	 2.0f,  2.0f, -2.0f,  1.0f, 0.0f, 0.0f,
	 2.0f,  2.0f, -2.0f,  1.0f, 0.0f, 0.0f,
	-2.0f,  2.0f, -2.0f,  1.0f, 0.0f, 0.0f,
	-2.0f, -2.0f, -2.0f,  1.0f, 0.0f, 0.0f,
// 前面
	-2.0f, -2.0f,  2.0f,  1.0f, 1.0f, 0.0f,
	 2.0f, -2.0f,  2.0f,  1.0f, 1.0f, 0.0f,
	 2.0f,  2.0f,  2.0f,  1.0f, 1.0f, 0.0f,
	 2.0f,  2.0f,  2.0f,  1.0f, 1.0f, 0.0f,
	-2.0f,  2.0f,  2.0f,  1.0f, 1.0f, 0.0f,
	-2.0f, -2.0f,  2.0f,  1.0f, 1.0f, 0.0f,
// 左面
	-2.0f,  2.0f,  2.0f,  0.0f, 1.0f, 0.0f,
	-2.0f,  2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
	-2.0f, -2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
	-2.0f, -2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
	-2.0f, -2.0f,  2.0f,  0.0f, 1.0f, 0.0f,
	-2.0f,  2.0f,  2.0f,  0.0f, 1.0f, 0.0f,
// 右面
	 2.0f,  2.0f,  2.0f,  0.0f, 1.0f, 1.0f,
	 2.0f,  2.0f, -2.0f,  0.0f, 1.0f, 1.0f,
	 2.0f, -2.0f, -2.0f,  0.0f, 1.0f, 1.0f,
	 2.0f, -2.0f, -2.0f,  0.0f, 1.0f, 1.0f,
	 2.0f, -2.0f,  2.0f,  0.0f, 1.0f, 1.0f,
	 2.0f,  2.0f,  2.0f,  0.0f, 1.0f, 1.0f,
// 下面
	-2.0f, -2.0f, -2.0f,  0.0f, 0.0f, 1.0f,
	 2.0f, -2.0f, -2.0f,  0.0f, 0.0f, 1.0f,
	 2.0f, -2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
	 2.0f, -2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
	-2.0f, -2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
	-2.0f, -2.0f, -2.0f,  0.0f, 0.0f, 1.0f,
// 上面
	-2.0f,  2.0f, -2.0f,  1.0f, 0.0f, 1.0f,
	 2.0f,  2.0f, -2.0f,  1.0f, 0.0f, 1.0f,
	 2.0f,  2.0f,  2.0f,  1.0f, 0.0f, 1.0f,
	 2.0f,  2.0f,  2.0f,  1.0f, 0.0f, 1.0f,
	-2.0f,  2.0f,  2.0f,  1.0f, 0.0f, 1.0f,
	-2.0f,  2.0f, -2.0f,  1.0f, 0.0f, 1.0f
};

const int width = 800, height = 600;

int main() {
	GLFWwindow* window;
	bool state;
	state = init_GLFW_window(window);
	if (!state) return -1;
	init_imgui(window);
	Shader MyShader = Shader();
	unsigned int VBO, VAO, EBO;
	configure_vertex_attributes(VBO, VAO, EBO);
	// 渲染循环
	bool is_ortho = true, pre_ortho = true, is_pers = false, pre_pers = false;
	// ortho 参数
	float left = -20.0f, right = 20.0f, top = 20.0f, bottom = -20.0f, o_near = 0.1f, o_far = 100.0f;
	// perspective参数
	float angle = 45.0f, p_near = 0.1f, p_far = 100.0f;
	while (!glfwWindowShouldClose(window)) {
		process_input(window);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::Checkbox("ortho", &is_ortho);
			if (is_ortho) {
				ImGui::SliderFloat("Left  ", &left, -20.0f, 0.0f);
				ImGui::SliderFloat("Right ", &right, 0.0f, 20.0f);
				ImGui::SliderFloat("Bottom", &bottom, -20.0f, 0.0f);
				ImGui::SliderFloat("Top   ", &top, 0.0f, 20.0f);
				ImGui::SliderFloat("Near  ", &o_near, 0.1f, 10.0f);
				ImGui::SliderFloat("Far   ", &o_far, 0.1f, 100.0f);
			}
			ImGui::Checkbox("perspective", &is_pers);
			if (is_pers) {
				ImGui::SliderFloat("Angle", &angle, 0.1f, 180.0f);
				ImGui::SliderFloat("Near ", &p_near, 0.1f, 10.0f);
				ImGui::SliderFloat("Far  ", &p_far, 0.1f, 100.0f);
			}
		}
		if (is_ortho && !pre_ortho) {
			pre_pers = is_pers = false;
			is_ortho = pre_ortho = true;
		}
		else if (is_pers && !pre_pers) {
			pre_ortho = is_ortho = false;
			is_pers = pre_pers = true;
		}
		// 变换
		ImGui::Render();
		// 3D	
		// modelview矩阵
		glm::mat4 modelview = glm::mat4(1.0f);
		modelview = glm::rotate(modelview, glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		modelview = glm::translate(modelview, glm::vec3(-0.5f, 0.5f, -1.5f));
		// view矩阵(camera)
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
		// 投影矩阵
		glm::mat4 projection = glm::mat4(1.0f);
		if (is_ortho) {
			projection = glm::ortho(left, right, bottom, top, o_near, o_far);
		}
		else if (is_pers) {
			projection = glm::perspective(glm::radians(angle), float(width) / height, p_near, p_far);
		}
		glUniformMatrix4fv(glGetUniformLocation(MyShader.ID, "modelview"), 1, GL_FALSE, glm::value_ptr(modelview));
		glUniformMatrix4fv(glGetUniformLocation(MyShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(MyShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// 渲染
		MyShader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glEnable(GL_DEPTH_TEST);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO);
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

void configure_vertex_attributes(unsigned int& VBO, unsigned int& VAO, unsigned int& EBO) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

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
