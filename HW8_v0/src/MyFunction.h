#pragma once
#include <stb_image.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GlobalVariables.h"
#include "Shader.h"
#include <cmath>

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
// 归一化坐标
void normalize_point(MyPoint& point) {
	int half_w = width / 2, half_h = height / 2;
	point.x = (float)(point.x - half_w) / width * 2;
	point.y = -(float)(point.y - half_h) / height * 2;
}
// 处理输入
void process_input(GLFWwindow* window) {
	if (is_press(window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
	}
}
// 处理鼠标位置事件
void cursor_position_callback(GLFWwindow* window, double x, double y) {
	if (canAddNewControlPoint) {
		selectedPoint.x = x;
		selectedPoint.y = y;
		selectedPoint.z = 0.0f;
		normalize_point(selectedPoint);
		control_points.push_back(selectedPoint);
		canAddNewControlPoint = false;
	}
}
// 处理鼠标点击事件
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS)
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			canAddNewControlPoint = true;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			if (!control_points.empty()) {
				control_points.pop_back();
			}
			break;
		}
}
// 将list转为数组
void convert_pointlist_to_arr(list<MyPoint> points, float*& arr) {
	if (arr) {
		delete arr;
		arr = NULL;
	}
	arr = new float[points.size() * 2];
	int index = 0;
	list<MyPoint>::iterator it = points.begin();
	for (; it != points.end(); it++, index += 2) {
		arr[index] = it->x;
		arr[index + 1] = it->y;
	}
}
// 将vector转为数组
void convert_pointvector_to_arr(vector<MyPoint> points, float*& arr, bool verbose = false) {
	if (arr) {
		delete[] arr;
		arr = NULL;
	}
	arr = new float[points.size() * 2];
	int index = 0;
	vector<MyPoint>::iterator it = points.begin();
	for (; it != points.end(); it++, index += 2) {
		arr[index] = it->x;
		arr[index + 1] = it->y;
		if (verbose) {
			cout << "( " << it->x << ", " << it->y << " )" << endl;
		}
	}
	if (verbose) cout << "-------------" << endl;
}
MyPoint get_curve_point(MyPoint A, MyPoint B, float ratio) {
	float x = ratio * B.x + (1 - ratio)*A.x;
	float y = ratio * B.y + (1 - ratio)*A.y;
	return MyPoint(x, y);
}
// 生成贝塞尔曲线
void generate_bezier_curve() {
	curve_points.clear();
	middle_points.clear();
	MyPoint endPoint(-1, -1, -1, true);
	if (control_points.size() <= 1) {
		return;
	}
	for (float i = 0.0; i <= 1; i += step) {
		auto tmp_ctr_points = control_points;
		tmp_ctr_points.push_back(endPoint);
		vector<MyPoint> temp;
		while (true) {
			if (tmp_ctr_points.size() == 2) break;
			MyPoint point1 = tmp_ctr_points.front();
			tmp_ctr_points.pop_front();
			MyPoint point2 = tmp_ctr_points.front();
			if (!point2.isEndPoint()) {
				MyPoint middlePoint = get_curve_point(point1, point2, i);
				tmp_ctr_points.push_back(middlePoint);
				if (i == currentRatio) {
					temp.push_back(middlePoint);
				}
			}
			else {
				tmp_ctr_points.pop_front();
				tmp_ctr_points.push_back(endPoint);
				if (i == currentRatio) {
					middle_points.push_back(temp);
					temp.clear();
				}
			}
		}
		curve_points.push_back(tmp_ctr_points.front());
	}
}
// 渲染控制点
void render_controlPoints() {
	if (control_points.size() == 0) return;
	if (controlVAO == 0) {
		glGenVertexArrays(1, &controlVAO);
		glGenBuffers(1, &controlVBO);
	}
	convert_pointlist_to_arr(control_points, control_points_arr);
	glBindVertexArray(controlVAO);
	glBindBuffer(GL_ARRAY_BUFFER, controlVBO);
	glBufferData(GL_ARRAY_BUFFER, control_points.size() * 2 * sizeof(float), control_points_arr, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(controlVAO);
	glPointSize(5.0f);
	glDrawArrays(GL_LINE_STRIP, 0, control_points.size());
	glDrawArrays(GL_POINTS, 0, control_points.size());
	glBindVertexArray(0);
}
// 渲染曲线
void render_curve() {
	if (control_points.size() <= 1) return;
	if (curveVAO == 0) {
		glGenVertexArrays(1, &curveVAO);
		glGenVertexArrays(1, &curveVBO);
	}
	generate_bezier_curve();
	convert_pointvector_to_arr(curve_points, curve_points_arr);
	glBindVertexArray(curveVAO);
	glBindBuffer(GL_ARRAY_BUFFER, curveVBO);
	glBufferData(GL_ARRAY_BUFFER, curve_points.size() * 2 * sizeof(float), curve_points_arr, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(curveVAO);
	glDrawArrays(GL_LINE_STRIP, 0, curve_points.size());
	glBindVertexArray(0);
}
// 渲染生成曲线的过程
void render_middle_process() {
	if (control_points.size() <= 1) return;
	if (middleVAO == 0) {
		glGenVertexArrays(1, &middleVAO);
		glGenBuffers(1, &middleVBO);
	}
	for (int i = 0; i < middle_points.size(); i++) {
		// 转成数组
		convert_pointvector_to_arr(middle_points[i], middle_points_arr);
		// 绑定
		glBindVertexArray(middleVAO);
		glBindBuffer(GL_ARRAY_BUFFER, middleVBO);
		glBufferData(GL_ARRAY_BUFFER, middle_points[i].size() * 2 * sizeof(float), middle_points_arr, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(middleVAO);
		glPointSize(5.0f);
		glDrawArrays(GL_POINTS, 0, middle_points[i].size());
		glDrawArrays(GL_LINE_STRIP , 0, middle_points[i].size());
		glBindVertexArray(0);

	}
	currentRatio += step;
	if (currentRatio >= 1.0) {
		currentRatio = step;
	}
}

#endif // !MYFUNCTION
