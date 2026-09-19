#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

#include "../mesh.h"

class Sphere : public Mesh
{
public:
    Sphere();
    void CreateSphere();
    Sphere(int sectors, int stacks, float radius);
    float getRadius() const { return radius_; }
    ~Sphere();
private:
    int sectors_, stacks_;
    float radius_;
};