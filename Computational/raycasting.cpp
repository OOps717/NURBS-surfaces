#include "raycasting.h"

#include <cmath>
#include <stdexcept>

Raycasting::Raycasting(double mouseX, double mouseY, int windowWidth, int windowHeight,
                        const glm::mat4& projection, const glm::mat4& view)
    : origin_(0.0f), direction_(0.0f), maxDistance_(0.0f)
{
    if (windowWidth <= 0 || windowHeight <= 0)
        throw std::invalid_argument("Raycasting requires positive window dimensions");

    const float x = static_cast<float>(2.0 * mouseX / windowWidth - 1.0);
    const float y = static_cast<float>(1.0 - 2.0 * mouseY / windowHeight);

    //Transform from Normalized Device Coordinates to world space
    const glm::mat4 inverse = glm::inverse(projection * view);
    glm::vec4 nearPoint = inverse * glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 farPoint = inverse * glm::vec4(x, y, 1.0f, 1.0f);
    nearPoint /= nearPoint.w;
    farPoint /= farPoint.w;
    origin_ = glm::vec3(nearPoint);
    const glm::vec3 delta = glm::vec3(farPoint) - origin_;
    maxDistance_ = glm::length(delta);
    direction_ = delta / maxDistance_;
}

bool Raycasting::intersectSphere(const glm::mat4& model, float radius, float& distance) const
{
    if (!(radius > 0.0f) || std::abs(glm::determinant(model)) < 1e-12f)
        return false;

    // Transform the ray into the sphere's local space, where the sphere is centred at the origin
    const glm::mat4 inverse = glm::inverse(model);
    const glm::vec3 origin = glm::vec3(inverse * glm::vec4(origin_, 1.0f));
    const glm::vec3 direction = glm::vec3(inverse * glm::vec4(direction_, 0.0f));

    // || x - c ||^2 = r^2, where x is a point on the ray, c is the sphere centre (0, 0, 0), and r is the radius
    // ax^2 + 2bx + c = 0, where x is the distance along the ray to the intersection point
    const float a = glm::dot(direction, direction);
    const float b = glm::dot(origin, direction);
    const float c = glm::dot(origin, origin) - radius * radius;
    const float discriminant = b * b - a * c;
    if (discriminant < 0.0f || a <= 0.0f) return false;

    const float root = std::sqrt(discriminant);
    float hit = (-b - root) / a;
    if (hit < 0.0f) hit = (-b + root) / a;
    if (!(hit >= 0.0f && hit <= maxDistance_)) return false;
    distance = hit;
    return true;
}
