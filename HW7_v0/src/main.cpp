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
	camera = Camera(width, height);
	Shader depthShader("./Shadow_Depth.vs", "./Shadow_Depth.fs");
	Shader debugDepthQuad("./Quad.vs", "./Quad.fs");
	Shader shader("./Shadow_Mapping.vs", "./Shadow_Mapping.fs");
	Shader lampShader("./lamp.vs", "./lamp.fs");
	// load texture
	unsigned int woodTexture = load_texture("./image/wood.png");
	// configure depth map FBO
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	const int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shader.use();
	shader.setInt("diffuseTexture", 0);
	shader.setInt("shadowMap", 1);
	debugDepthQuad.use();
	debugDepthQuad.setInt("depthMap", 0);

	glEnable(GL_DEPTH_TEST);
	bool show = false;
	bool ortho = true;
	while (!glfwWindowShouldClose(window)) {
		// 处理输入
		process_input(window);
		// 处理鼠标
		glfwSetCursorPosCallback(window, mouse_callback);
		// 处理滚轮
		glfwSetScrollCallback(window, scroll_callback);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		camera.updateSpeed();

		if (is_press(window, GLFW_KEY_SPACE)) {
			if (show) {
				show = false;
			}
			else {
				show = true;
			}
		}
		if (show) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		// ImGUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::Checkbox("ortho", &ortho);
		}
		ImGui::Render();
		
		

		glm::mat4 lightProjection(1.0f);
		glm::mat4 lightView(1.0f);
		float near_plane = 1.0f, far_plane = 7.5f;
		if (ortho)
			lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		else
			lightProjection = glm::perspective(glm::radians(45.0f), (float)width / height, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		depthShader.use();
		depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		// 1. render depth scene
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, woodTexture);
			glCullFace(GL_FRONT);
			render_scene(depthShader);
			glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 2. render scene
		shader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.getfov()), (float)width / (float)height, 0.1f, 100.0f);
		glm::mat4 view = camera.getViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setVec3("viewPos", camera.getCameraPos());
		shader.setVec3("lightPos", lightPos);
		shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		render_scene(shader);

		debugDepthQuad.use();
		if (ortho)
			debugDepthQuad.setInt("choice", 0);
		else
			debugDepthQuad.setInt("choice", 1);
		debugDepthQuad.setFloat("near_plane", near_plane);
		debugDepthQuad.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		// render_quad();

		// 设置光源
		lampShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lampShader.setMat4("model", model);
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);
		render_cube();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
	glfwTerminate();
	return 0;
}