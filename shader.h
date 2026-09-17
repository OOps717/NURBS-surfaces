#ifndef SHADER_H
#define SHADER_H

#include <cstring>
#include <GL/glew.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include "Light/directionalLight.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {

public:
    Shader();

    void CreateFromString(const char* vertexSource, const char* fragmentSource);
	void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation);
    void CreateFromFiles(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation);

    void validate();

    std::string ReadFile(const char* fileLocation);

    GLuint GetProjectionLocation() { return uniformProjection_; };
	GLuint GetModelLocation() { return uniformModel_; };
    GLuint GetViewLocation() { return uniformView_; };
    GLuint GetEyePositionLocation() { return uniformEyePosition_; }
    GLuint GetAmbientColourLocation() { return uniformDirectionalLight_.uniformColour_; };
    GLuint GetAmbientIntensityLocation() { return uniformDirectionalLight_.uniformAmbientIntensity_; };
    GLuint GetDirectionLocation() { return uniformDirectionalLight_.uniformDirection_; };
    GLuint GetDiffuseIntensityLocation() { return uniformDirectionalLight_.uniformDiffuseIntensity_; };
    GLuint GetShininessLocation() { return uniformShininess_; };
    GLuint GetSpecularIntensityLocation() { return uniformSpecularIntensity_; };
    GLuint GetShaderProgram() { return shaderProgram_; };

    void SetDirectionalLight(DirectionalLight * dLight);
	void SetDirectionalShadowMap(GLuint textureUnit);
	void SetDirectionalLightTransform(glm::mat4* lTransform);

    void UseShader();
    void ClearShader();

    ~Shader();
private:

    GLuint vertexShader_, geometryShader_, fragmentShader_, shaderProgram_;
    GLuint uniformModel_, uniformProjection_, uniformView_, uniformEyePosition_;
    GLuint uniformSpecularIntensity_, uniformShininess_;
    GLuint uniformDirectionalLightTransform_, uniformDirectionalShadowMap_;


    struct {
        GLuint uniformColour_;
        GLuint uniformAmbientIntensity_;
        GLuint uniformDiffuseIntensity_;
        GLuint uniformDirection_;
    } uniformDirectionalLight_;

    void CreateAndCompileShader(const char* vertexSource, const char* fragmentSource);
    void CreateAndCompileShader(const char* vertexSource, const char* geometrySource, const char* fragmentSource);
    GLuint AddShader(GLuint theProgram, const char* shaderSource, GLenum shaderType);

    void CompileProgram ();
};

#endif