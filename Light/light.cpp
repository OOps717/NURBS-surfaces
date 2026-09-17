#include "light.h"

Light::Light()
    : colour_(1.0f), ambientIntensity_(1.0f), diffuseIntensity_(0.0f),
        lightProj_(1.0f), shadowMap_(nullptr)
{
}

Light::Light(GLfloat shadowWidth, GLfloat shadowHeight, GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity)
    : colour_(red, green, blue), ambientIntensity_(aIntensity), diffuseIntensity_(dIntensity),
        lightProj_(1.0f), shadowMap_(new ShadowMap())
{
    shadowMap_->Init(shadowWidth, shadowHeight);
}

Light::~Light()
{
}
