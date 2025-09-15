#include "scene.h"

#include <glm/ext/matrix_transform.hpp>

std::optional<HitInfo> Scene::intersect(const Ray& ray, float tMin, float tMax) const
{
    std::optional<HitInfo>         closestHit{};
    const ShapeInstance* closestInstance{nullptr};
    for (const auto& instance : shape_) {
        // Transform the ray from world space to object space
        Ray  localRay = ray.transform(instance.inverseModelMatrix);
        auto hitInfo = instance.shape->intersect(localRay, tMin, tMax);
        if (hitInfo.has_value()) {
            closestHit = hitInfo;
            tMax = hitInfo->hitT;
            closestInstance = &instance;
        }
    }

    if (closestInstance) {
        closestHit->hitPos = closestInstance->modelMatrix * glm::vec4(closestHit->hitPos, 1.0f);
        closestHit->normal =
            glm::normalize(glm::vec3{glm::transpose(closestInstance->inverseModelMatrix) *
                                     glm::vec4(closestHit->normal, 0.0f)});
        closestHit->material = closestInstance->material;
    }

    return closestHit;
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
