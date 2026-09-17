#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

class Mesh {

public:
    Mesh();
    
    void createMesh(GLfloat *meshVertices, unsigned int *meshIndices, unsigned int numOfVertices, unsigned int numOfIndices);
    void createMesh(GLfloat *meshVertices, unsigned int *meshIndices, unsigned int numOfVertices, unsigned int numOfIndices, GLsizei stride, bool hasNormals = false);
    void renderInstancedMesh(GLuint count);
    void renderMesh();
    void clearMesh();

    GLfloat* computeVertexNormals(GLint vertexPasser);

    void translate(float x, float y, float z) { model_ = glm::translate(model_, glm::vec3(x, y, z)); };
    void rotateX(float angle) { rotateAboutAxe (angle, glm::vec3(1.0f, 0.0f, 0.0f)); };
    void rotateY(float angle) { rotateAboutAxe (angle, glm::vec3(0.0f, 1.0f, 0.0f)); };
    void rotateZ(float angle) { rotateAboutAxe (angle, glm::vec3(0.0f, 0.0f, 1.0f)); };
    void rotateAboutAxe(float angle, glm::vec3 axe);
    void scale(float sX, float sY, float sZ) { model_ = glm::scale(model_, glm::vec3(sX, sY, sZ)); };

    glm::mat4 getModelMatrix () { return model_; }; 
    glm::vec3 getPosition () { return glm::vec3(model_[3]); };
    glm::vec3 getScale () { return glm::vec3(model_[0][0], model_[1][1], model_[2][2]); };
    glm::vec3 getRotation ();
    GLuint getVAO () { return VAO_; }

    void reinitializeModel () { model_ = glm::mat4(1.0f);};
    GLfloat* normals;


    ~Mesh();
protected:
    GLuint VAO_, VBO_, EBO_;
    GLuint* indices_;
    GLfloat* vertices_;
    GLsizei indexCount_, vertexCount_;
    glm::mat4 model_;
};