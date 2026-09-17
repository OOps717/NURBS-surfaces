#include "material.h"

Material::Material() : Material(0.0f, 0.0f)
{
}

Material::Material(GLfloat sIntensity, GLfloat shine) : specularIntensity_(sIntensity), shininess_(shine) {};

void Material::useMaterial(GLuint specularIntensityLocation, GLuint shininessLocation)
{
	glUniform1f(specularIntensityLocation, specularIntensity_);
	glUniform1f(shininessLocation, shininess_);
}

Material::~Material() {};