#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);

	glm::vec3 getCameraPosition() { return position_; };
	glm::vec3 getCameraDirection() { return glm::normalize(direction_); };

	glm::mat4 calculateViewMatrix();

	~Camera();

private:
	glm::vec3 position_, direction_, up_, right_;
	glm::vec3 worldUp_;

	GLfloat yaw_, pitch_;
	GLfloat movementSpeed_, turnSpeed_;
	void update();
};
