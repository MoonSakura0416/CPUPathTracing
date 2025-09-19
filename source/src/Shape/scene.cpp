#include "pch.h"

#include "Shape/scene.h"

std::optional<HitInfo> Scene::intersect(const Ray& ray, float tMin, float tMax) const
{
    return bvh_.intersect(ray, tMin, tMax);
}

void Scene::addShape(std::shared_ptr<Shape> shape, std::shared_ptr<Material> material,
                     const glm::vec3& translation, const glm::vec3& scale,
                     const glm::vec3& rotation)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), translation) *
                      glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), {0, 0, 1}) *
                      glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), {0, 1, 0}) *
                      glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), {1, 0, 0}) *
                      glm::scale(glm::mat4(1.0f), scale);
    shape_.emplace_back(std::move(shape), std::move(material), model, glm::inverse(model));
}
