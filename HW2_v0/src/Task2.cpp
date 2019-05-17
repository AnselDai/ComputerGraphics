#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

using namespace std;

bool init_GLFW_window(GLFWwindow*& window);
bool setup_shader_program(unsigned int& shaderProgram);
void configure_vertex_attributes(unsigned int& VBO, unsigned int& VAO);
void change_triangle_color(unsigned int shaderProgram, ImVec4 triangleColor);
void process_input(GLFWwindow* window);

float vertices[] = {
	// 位置            
	 0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 右下
	-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // 左下
	 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f  // 顶部
};

int main() {
	GLFWwindow* window;
	bool state;
	state = init_GLFW_window(window);
	if (!state) return -1;
	unsigned int shaderProgram;
	state = setup_shader_program(shaderProgram);
	if (!state) return -1;
	unsigned int VBO, VAO;
	configure_vertex_attributes(VBO, VAO);
	glUseProgram(shaderProgram);
	while (!glfwWindowShouldClose(window)) {
		process_input(window);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
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
	const int width = 800, height = 600;
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

bool setup_shader_program(unsigned int& shaderProgram) {
	// 1. 编写顶点着色器
	const char* vertexShaderCode = "#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aColor;\n"
		"out vec3 ourColor;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos, 1.0);\n"
		"   ourColor = aColor;\n"
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
		"in vec3 ourColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(ourColor, 1.0f);\n"
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

