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
	-2.0f, -2.0f,  2.0f,  1.0f, 0.0f, 0.0f,
	 2.0f, -2.0f,  2.0f,  1.0f, 0.0f, 0.0f,
	 2.0f,  2.0f,  2.0f,  1.0f, 0.0f, 0.0f,
	 2.0f,  2.0f,  2.0f,  1.0f, 0.0f, 0.0f,
	-2.0f,  2.0f,  2.0f,  1.0f, 0.0f, 0.0f,
	-2.0f, -2.0f,  2.0f,  1.0f, 0.0f, 0.0f,
// 左面
	-2.0f,  2.0f,  2.0f,  0.0f, 1.0f, 0.0f,
	-2.0f,  2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
	-2.0f, -2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
	-2.0f, -2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
	-2.0f, -2.0f,  2.0f,  0.0f, 1.0f, 0.0f,
	-2.0f,  2.0f,  2.0f,  0.0f, 1.0f, 0.0f,
// 右面
	 2.0f,  2.0f,  2.0f,  0.0f, 1.0f, 0.0f,
	 2.0f,  2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
	 2.0f, -2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
	 2.0f, -2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
	 2.0f, -2.0f,  2.0f,  0.0f, 1.0f, 0.0f,
	 2.0f,  2.0f,  2.0f,  0.0f, 1.0f, 0.0f,
// 下面
	-2.0f, -2.0f, -2.0f,  0.0f, 0.0f, 1.0f,
	 2.0f, -2.0f, -2.0f,  0.0f, 0.0f, 1.0f,
	 2.0f, -2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
	 2.0f, -2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
	-2.0f, -2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
	-2.0f, -2.0f, -2.0f,  0.0f, 0.0f, 1.0f,
// 上面
	-2.0f,  2.0f, -2.0f,  0.0f, 0.0f, 1.0f,
	 2.0f,  2.0f, -2.0f,  0.0f, 0.0f, 1.0f,
	 2.0f,  2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
	 2.0f,  2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
	-2.0f,  2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
	-2.0f,  2.0f, -2.0f,  0.0f, 0.0f, 1.0f
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
	bool is_trans = false, is_rotate = false, is_scale = false;
	bool pre_istrans = false, pre_isrotate = false, pre_isscale = false;
	bool ishor = false, isver = false;
	bool pre_ishor = false, pre_isver = false;
	bool is_deep_test = false;
	bool direction = true;
	/*
		true：向上或者向右
		false：向下或者向左
	*/
	bool scale = true;
	/*
		true：放大
		false：缩小
	*/
	glm::vec3 pre_step_h = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 pre_step_v = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 pre_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	while (!glfwWindowShouldClose(window)) {
		process_input(window);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::Checkbox("Need Deep Test", &is_deep_test);
			if (is_deep_test) {
				ImGui::Checkbox("Translate", &is_trans);
				if (is_trans) {
					ImGui::Checkbox("Horizontally", &ishor);
					ImGui::Checkbox("Vertically", &isver);
				}
				ImGui::Checkbox("Rotate", &is_rotate);
				ImGui::Checkbox("Scale", &is_scale);
			}
		}

		if (is_deep_test) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else {
			is_trans = is_rotate = is_scale = false;
			pre_istrans = pre_isrotate = pre_isscale = false;
			ishor = pre_ishor = isver = pre_isver = false;
		}
		if (is_trans && !pre_istrans) {
			pre_istrans = true;
			pre_isrotate = is_rotate = pre_isscale = is_scale = false;
		}
		else if (is_rotate && !pre_isrotate) {
			pre_isrotate = true;
			pre_istrans = is_trans = pre_isscale = is_scale = false;
		}
		else if (is_scale && !pre_isscale) {
			pre_isscale = true;
			pre_istrans = is_trans = pre_isrotate = is_rotate = false;
		}
		glm::vec3 pre_step;
		if (is_trans) {
			if (ishor && !pre_ishor) {
				pre_ishor = true;
				isver = pre_isver = false;
				pre_step = glm::vec3(0.0f, 0.0f, 0.0f);
			}
			else if (isver && !pre_isver) {
				pre_isver = true;
				ishor = pre_ishor = false;
				pre_step = glm::vec3(0.0f, 0.0f, 0.0f);
			}
		}


		// 变换
		glm::mat4 trans = glm::mat4(1.0f);
		if (is_trans) {
			if (ishor) {
				glm::vec3 p_step = glm::vec3(0.5f, 0.0f, 0.0f), n_step = glm::vec3(-0.5f, 0.0f, 0.0f);
				glm::vec4 p_axis = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), n_axis = glm::vec4(-0.1f, -0.1f, -0.1f, 1.0f);
				if (direction) {
					glm::vec3 step = pre_step_h + p_step;
					glm::vec4 next = glm::translate(trans, step)*p_axis;
					if (next.x <= 20.0f) {
						trans = glm::translate(trans, step);
						pre_step_h = step;
					}
					else {
						step = pre_step_h + n_step;
						trans = glm::translate(trans, step);
						pre_step_h = step;
						direction = false;
					}
				}
				else {
					glm::vec3 step = pre_step_h + n_step;
					glm::vec4 next = glm::translate(trans, step)*n_axis;
					if (next.x >= -20.0f) {
						trans = glm::translate(trans, step);
						pre_step_h = step;
					}
					else {
						step = pre_step_h + p_step;
						trans = glm::translate(trans, step);
						pre_step_h = step;
						direction = true;
					}
				}
			}
			else if (isver) {
				glm::vec3 p_step = glm::vec3(0.0f, 0.05f, 0.0f), n_step = glm::vec3(0.0f, -0.05f, 0.0f);
				glm::vec4 p_axis = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), n_axis = glm::vec4(-0.1f, -0.1f, -0.1f, 1.0f);
				if (direction) {
					glm::vec3 step = pre_step_v + p_step;
					glm::vec4 next = glm::translate(trans, step)*p_axis;
					if (next.y <= 1.0f) {
						trans = glm::translate(trans, step);
						pre_step_v = step;
					}
					else {
						step = pre_step_v + n_step;
						trans = glm::translate(trans, step);
						pre_step_v = step;
						direction = false;
					}
				}
				else {
					glm::vec3 step = pre_step_v + n_step;
					glm::vec4 next = glm::translate(trans, step)*n_axis;
					if (next.y >= -1.0f) {
						trans = glm::translate(trans, step);
						pre_step_v = step;
					}
					else {
						step = pre_step_v + p_step;
						trans = glm::translate(trans, step);
						pre_step_v = step;
						direction = true;
					}
				}
			}
		}
		else if (is_rotate) {
			trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, 15.0f));
			trans = glm::rotate(trans, (float)glfwGetTime()*glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, -15.0f));
		}
		else if (is_scale) {
			glm::vec3 tmp_scale;
			float p_scale = 1.1, n_scale = 0.95;
			glm::vec4 axis = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
			if (scale) {
				tmp_scale = pre_scale * p_scale;
				if (tmp_scale.x <= 3.0f && tmp_scale.y <= 3.0f) {
					trans = glm::scale(trans, tmp_scale);
					pre_scale = tmp_scale;
				}
				else {
					scale = false;
					trans = glm::scale(trans, pre_scale);
				}
			}
			else {
				tmp_scale = pre_scale * n_scale;
				if (tmp_scale.x >= 0.8 && tmp_scale.y >= 0.8) {
					trans = glm::scale(trans, tmp_scale);
					pre_scale = tmp_scale;
				}
				else {
					scale = true;
					trans = glm::scale(trans, pre_scale);
				}
			}
		}
		ImGui::Render();
		glUniformMatrix4fv(glGetUniformLocation(MyShader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
// 3D	
	// 模型矩阵
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		// 观察矩阵
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -15.0f));
		// 投影矩阵
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), float(width) / height, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(MyShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(MyShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(MyShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// 渲染
		MyShader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		if (is_deep_test)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

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
