#include "sphere.h"
#include <cmath>
#include <iostream>

Sphere::Sphere() : Sphere(36, 18, 1.0f) {
}

Sphere::Sphere(int sectors, int stacks, float radius)
    : Mesh(), sectors_(sectors), stacks_(stacks), radius_(radius) {
}

void Sphere::CreateSphere() {
    std::vector<GLfloat> verticesInfo;  // 8 данных для каждой вершины
    std::vector<GLuint> indicesInfo;

    float sectorStep = 2 * M_PI / sectors_;
    float stackStep = M_PI / stacks_;

    for (int i = 0; i <= stacks_; ++i) {
        float stackAngle = M_PI / 2 - i * stackStep;  // Угол по широте
        float xy = radius_ * cosf(stackAngle);         // Радиус на данной широте
        float z = radius_ * sinf(stackAngle);          // Высота на данной широте

        for (int j = 0; j <= sectors_; ++j) {
            float sectorAngle = j * sectorStep;   // Угол по долготе

            // Позиция вершины
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            // Нормаль (нормализованное направление)
            float nx = x / radius_;
            float ny = y / radius_;
            float nz = z / radius_;

            // Текстурные координаты
            float s = (float)j / sectors_;
            float t = (float)i / stacks_;

            // Формат Mesh: x, y, z, u, v, nx, ny, nz
            verticesInfo.push_back(x);
            verticesInfo.push_back(y);
            verticesInfo.push_back(z);
            verticesInfo.push_back(s);
            verticesInfo.push_back(t);
            verticesInfo.push_back(nx);
            verticesInfo.push_back(ny);
            verticesInfo.push_back(nz);
        }
    }
    
    for (int i = 0; i < stacks_; ++i) {
        int k1 = i * (sectors_ + 1);  
        int k2 = k1 + sectors_ + 1;  

        for (int j = 0; j < sectors_; ++j, ++k1, ++k2) {
            if (i != 0) {
                // Первый треугольник
                indicesInfo.push_back(k1);
                indicesInfo.push_back(k2);
                indicesInfo.push_back(k1 + 1);
            }
            if (i != (stacks_ - 1)) {
                // Второй треугольник
                indicesInfo.push_back(k1 + 1);
                indicesInfo.push_back(k2);
                indicesInfo.push_back(k2 + 1);
            }
        }
    }

    createMesh(verticesInfo.data(), indicesInfo.data(),
               static_cast<unsigned int>(verticesInfo.size()),
               static_cast<unsigned int>(indicesInfo.size()), 8 * sizeof(GLfloat), true);
}

Sphere::~Sphere() {
}