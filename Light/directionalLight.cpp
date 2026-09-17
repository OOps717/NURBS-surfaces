#include "directionalLight.h"

DirectionalLight::DirectionalLight() : Light(), direction_(0.0f, -1.0f, 0.0f)
{
	float near_plane = 0.01f, far_plane = 20.5f;
	lightProj_ = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane); 
}

DirectionalLight::DirectionalLight(GLfloat shadowWidth, GLfloat shadowHeight,
									GLfloat red, GLfloat green, GLfloat blue,
									GLfloat aIntensity, GLfloat dIntensity,
									GLfloat xDir, GLfloat yDir, GLfloat zDir)
    : Light(shadowWidth, shadowHeight, red, green, blue, aIntensity, dIntensity),
      direction_(xDir, yDir, zDir)
{
	float near_plane = 0.01f, far_plane = 40.5f;
	lightProj_ = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane); 
}

void DirectionalLight::UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
	GLfloat diffuseIntensityLocation, GLfloat directionLocation)
{
	glUniform3f(ambientColourLocation, colour_.x, colour_.y, colour_.z);
	glUniform1f(ambientIntensityLocation, ambientIntensity_);

	glUniform3f(directionLocation, direction_.x, direction_.y, direction_.z);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity_);
}

glm::mat4 DirectionalLight::CalculateLightTransform()
{
	return lightProj_ * glm::lookAt(-direction_, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

DirectionalLight::~DirectionalLight()
{
}