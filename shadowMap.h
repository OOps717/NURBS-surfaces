#pragma once

#include <GL/glew.h>
#include <iostream>

class ShadowMap
{
public:
	ShadowMap();

	virtual bool Init(GLuint width, GLuint height);

	virtual void Write();

	virtual void Read(GLenum textureUnit);

	GLuint GetShadowWidth() { return shadowWidth_; }
	GLuint GetShadowHeight() { return shadowHeight_; }

	~ShadowMap();

protected:
	GLuint FBO_, shadowMap_;
	GLuint shadowWidth_, shadowHeight_;
};
