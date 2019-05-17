#include "Camera.h"

void Camera::moveForward()
{
	cameraPos += cameraSpeed * cameraFront;
}

void Camera::moveBack()
{
	cameraPos -= cameraSpeed * cameraFront;
}

void Camera::moveLeft()
{
	cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void Camera::moveRight()
{
	cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void Camera::scroll(double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f) fov -= yoffset;
	if (fov <= 1.0f) fov = 1.0f;
	if (fov >= 45.0f) fov = 45.0f;
}

void Camera::moveMouse(double xpos, double ypos)
{
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 180.0f)
		pitch = 180.0f;
	if (pitch < -180.0f)
		pitch = -180.0f;
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
}

void Camera::updateSpeed()
{
	float currentFrame = glfwGetTime();
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	cameraSpeed = 4.0f * deltaTime;
}
