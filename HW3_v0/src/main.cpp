#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <utility>
#include <vector>
#include "MyClass.h"

using namespace std;

bool init_GLFW_window(GLFWwindow*& window);
void init_imgui(GLFWwindow* window);
bool setup_shader_program(unsigned int& shaderProgram);
void configure_vertex_attributes(unsigned int& VBO, unsigned int& VAO);
void process_input(GLFWwindow* window);
void generateVertices(Point p1, Point p2, Point p3);
void bresenham(Point p1, Point p2, vector<Point>& vertices);
void bresenham_circle(Point center, int radius);
void triangle_resterization(Point p[3], vector<Point>& v);
void get_triangle_bbox(Point p[3], Point& min, Point& max);
bool is_in_triangle(Point point, Point p[3]);

float* vertices = NULL;
int s;
int width = 800, height = 600;
bool need_raterization = false, pre_need_rat = false;

int main() {
	GLFWwindow* window;
	bool state;
	state = init_GLFW_window(window);
	if (!state) return -1;
	init_imgui(window);
	unsigned int shaderProgram;
	state = setup_shader_program(shaderProgram);
	if (!state) return -1;
	unsigned int VBO, VAO;
	bool line = true, circle = false, pre_line = true, pre_circle = false, init = true;
	need_raterization = pre_need_rat = false;
	while (!glfwWindowShouldClose(window)) {
		process_input(window);
		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		static int r = 80, pre_r = 80;
		ImGui::NewFrame();
		{
			ImGui::Checkbox("Line", &line);
			if (line)
				ImGui::Checkbox("Raterization", &need_raterization);
			ImGui::Checkbox("Circle", &circle);
			if (circle)
				ImGui::SliderInt("Radius", &r, 0, 400);
		}
		ImGui::Render();
		if (init || (!pre_line && line)) {
			Point p1(-0.8f*width / 2, -0.5f*height / 2), p2(0.8f*width / 2, -0.5f*height / 2), p3(0.0f*width / 2, 0.8f*height / 2);
			generateVertices(p1, p2, p3);
			pre_line = line = true;
			pre_circle = circle = false;
			if (init) init = false;
		}
		else if (pre_need_rat != need_raterization) {
			Point p1(-0.8f*width / 2, -0.5f*height / 2), p2(0.8f*width / 2, -0.5f*height / 2), p3(0.0f*width / 2, 0.8f*height / 2);
			generateVertices(p1, p2, p3);
			pre_line = line = true;
			pre_circle = circle = false;
			pre_need_rat = need_raterization;
			if (init) init = false;
		}
		else if (pre_r != r || (!pre_circle && circle)) {
			bresenham_circle(Point(-0.2f*width, -0.2*height), r);
			pre_line = line = false;
			pre_circle = circle = true;
			pre_r = r;
		}
		configure_vertex_attributes(VBO, VAO);
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glPointSize(10.0f);
		int num = s / 12;
		glDrawArrays(GL_POINTS, 0, num);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	if (vertices) delete vertices;
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
	window = glfwCreateWindow(width, height, "Homework 3", NULL, NULL);
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

bool setup_shader_program(unsigned int& shaderProgram) {
	// 1. 编写顶点着色器
	const char* vertexShaderCode = "#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos, 1.0);\n"
		"}\0";
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
		return false;
	}
	// 4. 编写片段着色器
	const char* fragmentShaderCode = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\0";
	// 5. 编译片段着色器
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragmentShader);
	// 6. 检查编译是否成功
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cout << "ERROR::FRAGMENT::COMPILE" << endl << infoLog << endl;
		return false;
	}
	// 6. 将两个着色器附加到一个着色器程序上
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// 7. 检查链接是否失败
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		cout << "ERROR::PROGRAM::LINK" << endl << infoLog << endl;
		return false;
	}
	// 8. 删除两个着色器对象
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return true;
}

void configure_vertex_attributes(unsigned int& VBO, unsigned int& VAO) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, s, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void generateVertices(Point p1, Point p2, Point p3) {
	vector<Point> v;
	bresenham(p1, p2, v);
	bresenham(p1, p3, v);
	bresenham(p2, p3, v);
	Point head[3] = { p1, p2, p3 };
	if (need_raterization) triangle_resterization(head, v);
	s = v.size() * 3 * 4;
	if (vertices) delete vertices;
	vertices = new float[s/4];
	int index = 0;
	for (int i = 0; i < v.size(); i++, index+=3) {
		vertices[index] = v[i].x/float(width/2);
		vertices[index + 1] = v[i].y/float(height/2);
		vertices[index + 2] = 0.0f;
	}
}

void bresenham(Point p1, Point p2, vector<Point>& vertices) {
	// 决定起点和终点
	Point s, e;
	if          (p1.x > p2.x) { s = p2; e = p1; }
	else if     (p1.x < p2.x) { s = p1; e = p2; }
	else {
		if      (p1.y < p2.y) { s = p1; e = p2; }
		else if (p1.y > p2.y) { s = p2; e = p1; }
		else {
			vertices.push_back(p1);
			return;
		}
	}
	// 初始化Δx，Δy
	float delta_x, delta_y;
	delta_x = e.x - s.x;
	delta_y = e.y - s.y;
	int x_step = 20, y_step = x_step*abs(delta_y/delta_x);
	float t, p0, x, y, p;
	if (delta_x > 0 && delta_y > 0) {
		t = 2 * delta_y - 2 * delta_x;
		p0 = 2 * delta_y - delta_x;
		x = s.x;  y = s.y;  p = p0;
		vertices.push_back(s);
		while (x + x_step < e.x) {
			if (p <= 0) {
				Point point(x + x_step, y);
				vertices.push_back(point);
				p = p + 2 * delta_y;
				x = x + x_step;
			}
			else {
				Point point(x + x_step, y + y_step);
				vertices.push_back(point);
				p = p + t;
				x = x + x_step;
				y = y + y_step;
			}
		}
		vertices.push_back(e);
	}
	else if (delta_x > 0 && delta_y < 0) {
		t = 2 * delta_y + 2 * delta_x;
		p0 = 2 * delta_y + delta_x;
		x = s.x; y = s.y; p = p0;
		vertices.push_back(s);
		while (x + x_step < e.x) {
			if (p <= 0) {
				Point point(x + x_step, y - y_step);
				vertices.push_back(point);
				p = p + t;
				x = x + x_step;
				y = y - y_step;
			}
			else {
				Point point(x + x_step, y);
				vertices.push_back(point);
				p = p + 2 * delta_y;
				x = x + x_step;
			}
		}
		vertices.push_back(e);
	}
	else if (delta_x > 0 && delta_y == 0) {
		x = s.x; y = s.y;
		vertices.push_back(s);
		while (x + x_step < e.x) {
			Point point(x + x_step, y);
			vertices.push_back(point);
			x = x + x_step;
		}
		vertices.push_back(e);
	}
	else if (delta_x == 0 && delta_y > 0) {
		x = s.x; y = s.y;
		vertices.push_back(s);
		cout << s.x << " " << s.y << endl;
		while (y + y_step < e.y) {
			Point point(x, y + y_step);
			vertices.push_back(point);
			y = y + y_step;
		}
		vertices.push_back(e);
	}
}

void triangle_resterization(Point p[3], vector<Point>& v) {
	Point min, max;
	get_triangle_bbox(p, min, max);
	for (int y = min.y; y < max.y; y++) {
		for (int x = min.x; x < max.x; x++) {
			if (is_in_triangle(Point(x, y), p)) {
				v.push_back(Point(x, y));
			}
		}
	}
}

void get_triangle_bbox(Point p[3], Point& min, Point& max) {
	min = p[0];
	max = p[0];
	for (int i = 1; i < 3; i++) {
		if (p[i].x > max.x) max.x = p[i].x;
		else if (p[i].x < min.x) min.x = p[i].x;
		if (p[i].y > max.y) max.y = p[i].y;
		else if (p[i].y < min.y) min.y = p[i].y;
	}
}

bool is_in_triangle(Point point, Point p[3]) {
	float x0 = p[2].x - p[0].x, y0 = p[2].y - p[0].y;   // v0
	float x1 = p[1].x - p[0].x, y1 = p[1].y - p[0].y;   // v1
	float x2 = point.x - p[0].x, y2 = point.y - p[0].y; // v2

	float t00 = x0 * x0 + y0 * y0; // v0*v0
	float t01 = x0 * x1 + y0 * y1; // v0*v1
	float t02 = x0 * x2 + y0 * y2; // v0*v2
	float t11 = x1 * x1 + y1 * y1; // v1*v1
	float t12 = x1 * x2 + y1 * y2; // v1*v2

	// u = ((v1*v1) * (v0*v2) - (v0*v1) * (v1*v2)) / ((v0*v0) * (v1*v1) - (v0*v1) * (v0*v1))
	float u = float(t11*t02 - t01 * t12) / (t00*t11 - t01 * t01);
	// v = ((v0*v0) * (v1*v2) - (v0*v1) * (v0*v2)) / ((v0*v0) * (v1*v1) - (v0*v1) * (v0*v1))
	float v = float(t00*t12 - t01 * t02) / (t00*t11 - t01 * t01);
	if (u + v <= 1 && u >= 0 && v >= 0) return true;
	else return false;
}

void bresenham_circle(Point center, int radius) {
	vector<Point> v;
	float d = 3 - 2 * radius;
	int x = 0, y = radius;
	while (x <= y) {
		v.push_back(Point(x, y));
		v.push_back(Point(y, x));
		v.push_back(Point(-y, x));
		v.push_back(Point(-x, y));
		v.push_back(Point(-x, -y));
		v.push_back(Point(-y, -x));
		v.push_back(Point(y, -x));
		v.push_back(Point(x, -y));
		if (d < 0) {
			d += 4 * x + 6;
		}
		else {
			d += 4 * (x - y) + 10;
			y-=10;
		}
		x+=10;
	}
	for (int i = 0; i < v.size(); i++) {
		v[i].x += center.x;
		v[i].y += center.y;
	}
	s = v.size() * 3 * 4;
	if (vertices) delete vertices;
	vertices = new float[s / 4];
	int index = 0;
	for (int i = 0; i < v.size(); i++, index+=3) {
		vertices[index] = v[i].x / float(width/2);
		vertices[index + 1] = v[i].y / float(height/2);
		vertices[index + 2] = 0.0f;
	}
}