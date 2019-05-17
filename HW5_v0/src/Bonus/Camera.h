#pragma once
#ifndef CAMERA
#define CAMERA

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
	void moveForward();
	void moveBack();
	void moveLeft();
	void moveRight();
	void scroll(double xoffset, double yoffset);
	void moveMouse(double xpos, double ypos);
	void updateSpeed();
	glm::vec3 getCameraPos() { return cameraPos; }
	glm::vec3 getCameraTarget() { return cameraPos + cameraFront; }
	glm::vec3 getCameraUp() { return cameraUp; }
	float getfov() { return fov; }
	Camera(float width, float height)
	{
		// move
		cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		// speed
		lastFrame = 0.0f;
		// mouse
		yaw = -90.0f;
		pitch = 0.0f;
		lastX = width / 2;
		lastY = height / 2;
		firstMouse = true;
		// scroll
		fov = 45.0f;
	}
private:
	glm::vec3 cameraPos, cameraFront, cameraUp;
	float lastX, lastY;
	bool firstMouse;
	float lastFrame;
	float cameraSpeed;
	float yaw, pitch;
	float fov;
};

#endif // !CAMERA
