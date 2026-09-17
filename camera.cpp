#include "camera.h"

Camera::Camera()
    : Camera(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 3.0f, 0.1f)
{
}

Camera::Camera(glm::vec3 initPosition, glm::vec3 initUp, GLfloat initYaw, GLfloat initPitch, GLfloat initMovementSpeed, GLfloat initTurnSpeed)
    : position_(initPosition),
        direction_(0.0f, 0.0f, -1.0f),
        up_(0.0f, 1.0f, 0.0f),
        right_(1.0f, 0.0f, 0.0f),
        worldUp_(initUp),
        yaw_(initYaw),
        pitch_(initPitch),
        movementSpeed_(initMovementSpeed),
        turnSpeed_(initTurnSpeed)
{
    update();
}

void Camera::keyControl (bool* keys, GLfloat deltaTime)
{
    GLfloat velocity = movementSpeed_ * deltaTime;
    if (keys[GLFW_KEY_W] /*|| keys[GLFW_KEY_UP]*/)
    {
        position_ += direction_ * velocity;
    }
    if (keys[GLFW_KEY_A] /*|| keys[GLFW_KEY_LEFT]*/)
    {
        position_ += -right_ * velocity;
    }
    if (keys[GLFW_KEY_S] /*|| keys[GLFW_KEY_DOWN]*/)
    {
        position_ += -direction_ * velocity;
    }
    if (keys[GLFW_KEY_D] /*|| keys[GLFW_KEY_RIGHT]*/)
    {
        position_ += right_ * velocity;
    }
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed_;
	yChange *= turnSpeed_;

	yaw_ += xChange;
	pitch_ += yChange;

	if (pitch_ > 89.0f)
	{
		pitch_ = 89.0f;
	}

	if (pitch_ < -89.0f)
	{
		pitch_ = -89.0f;
	}

	update();
}

glm::mat4 Camera::calculateViewMatrix()
{
    return glm::lookAt(position_, position_+direction_, up_);
}

void Camera::update() {
    direction_.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    direction_.y = sin(glm::radians(pitch_));
    direction_.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    direction_ = glm::normalize(direction_);

    right_ = glm::normalize(glm::cross(direction_, worldUp_));
    up_ = glm::normalize(glm::cross(right_, direction_));
}

Camera::~Camera()
{
}
