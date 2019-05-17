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
#include "GlobalVariables.h"
#include "MyFunction.h"

using namespace std;

int main() {
	GLFWwindow* window;
	bool state;
	state = init_GLFW_window(window);
	if (!state) return -1;
	init_imgui(window);
	camera = Camera(width, height);
	unsigned int cubeVAO, VBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	Shader Phong_lightingShader("./Phong_shading.vs", "./Phong_shading.fs");
	Shader Gouraud_lightingShader("./Gouraud_shading.vs", "./Gouraud_shading.fs");
	Shader lampShader("./lamp.vs", "./lamp.fs");
	float lightPosX = lightPos.x, lightPosY = lightPos.y, lightPosZ = lightPos.z;
	float cameraPosX = camera.getCameraPos().x, cameraPosY = camera.getCameraPos().y, cameraPosZ = camera.getCameraPos().z;
	float viewAngle = 50.0f;
	bool autoChange = false;
	bool phong = true, pre_phong = true, gouraud = false, pre_gouraud = false;
	float ambient = 0.1, diffuse = 1.0, specular = 0.5;
	int reflect = 32;
	while (!glfwWindowShouldClose(window)) {
		// 处理输入
		process_input(window);
		// 处理鼠标
		// glfwSetCursorPosCallback(window, mouse_callback);
		// 处理滚轮
		// glfwSetScrollCallback(window, scroll_callback);
		// 隐藏鼠标
		// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ImgUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::Checkbox("Phong Shading", &phong);
			ImGui::Checkbox("Gouraud Shading", &gouraud);
			ImGui::Checkbox("Autoly change", &autoChange);
			if (!autoChange) {
				ImGui::SliderFloat("light Position - x", &lightPosX, -5.0f, 5.0f);
				ImGui::SliderFloat("light Position - y", &lightPosY, -5.0f, 5.0f);
				ImGui::SliderFloat("light Position - z", &lightPosZ, -5.0f, 5.0f);
				ImGui::SliderFloat("camera Position - x", &cameraPosX, -5.0f, 5.0f);
				ImGui::SliderFloat("camera Position - y", &cameraPosY, -5.0f, 5.0f);
				ImGui::SliderFloat("camera Position - z", &cameraPosZ, -5.0f, 5.0f);
				ImGui::SliderFloat("view Angle", &viewAngle, 0.0f, 360.0f);
				ImGui::SliderFloat("ambient", &ambient, 0.0f, 1.0f);
				ImGui::SliderFloat("diffuse", &diffuse, 0.0f, 1.0f);
				ImGui::SliderFloat("specular", &specular, 0.0f, 1.0f);
				ImGui::SliderInt("reflect", &reflect, 10, 50);
			}
		}
		ImGui::Render();
		if (autoChange) {
			// 实时改变光源位置
			lightPos.x = sin(glfwGetTime()) * 1.0f;
			lightPos.y = cos(glfwGetTime()) * 1.0f;
		}
		else {
			lightPos = glm::vec3(lightPosX, lightPosY, lightPosZ);
			camera.setCameraPos(glm::vec3(cameraPosX, cameraPosY, cameraPosZ));
		}
		if (phong && !pre_phong) {
			phong = pre_phong = true;
			gouraud = pre_gouraud = false;
		}
		else if (gouraud && !pre_gouraud) {
			gouraud = pre_gouraud = true;
			phong = pre_phong = false;
		}

		Shader& lightingShader = phong ? Phong_lightingShader : Gouraud_lightingShader;

		camera.updateSpeed();
		// 模型矩阵
		glm::mat4 model;
		// 观察矩阵
		glm::mat4 view = camera.getViewMatrix();
		// 投影矩阵
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.getfov()), float(width) / height, 0.1f, 100.0f);
		
		// 设置lightShader
		lightingShader.use();
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("lightPos", lightPos);
		lightingShader.setVec3("viewPos", camera.getCameraPos());
		lightingShader.setFloat("ambientStrength", ambient);
		lightingShader.setFloat("diffuseStrength", diffuse);
		lightingShader.setFloat("specularStregth", specular);
		lightingShader.setInt("reflectExtent", reflect);
		model = glm::mat4(1.0f);
		// model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(viewAngle), glm::vec3(0.5f, 1.0f, 0.0f));
		// model = glm::scale(model, glm::vec3(0.5f));
		lightingShader.setMat4("model", model);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("projection", projection);
		// 渲染物体
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// 设置lampShader
		lampShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		// model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		lampShader.setMat4("model", model);
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);
		// 渲染光源
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glEnable(GL_DEPTH_TEST);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}