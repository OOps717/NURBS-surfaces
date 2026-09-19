#pragma once

#include <glm/glm.hpp>

class Raycasting {
public:
    // Cursor and window dimensions must use the same coordinate units.
    Raycasting(double mouseX, double mouseY, int windowWidth, int windowHeight,
                const glm::mat4& projection, const glm::mat4& view);

    // Sphere is centred at the local origin; model includes all parent transforms.
    // Distance is measured from the near clipping plane along the world-space ray.
    bool intersectSphere(const glm::mat4& model, float radius, float& distance) const;

    const glm::vec3& getOrigin() const { return origin_; }
    const glm::vec3& getDirection() const { return direction_; }

private:
    glm::vec3 origin_;
    glm::vec3 direction_;
    float maxDistance_;
};
