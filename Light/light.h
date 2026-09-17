#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../shadowMap.h"

class Light
{
public:
	Light();
	Light(GLfloat shadowWidth, GLfloat shadowHeight,
			GLfloat red, GLfloat green, GLfloat blue, 
			GLfloat aIntensity, GLfloat dIntensity);

	ShadowMap* GetShadowMap() { return shadowMap_; }

	~Light();

protected:
	glm::vec3 colour_;
	GLfloat ambientIntensity_;
	GLfloat diffuseIntensity_;

	glm::mat4 lightProj_;

	ShadowMap* shadowMap_;
};

