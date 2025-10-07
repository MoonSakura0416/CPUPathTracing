#include "pch.h"

#include "Shape/scene.h"

#include "Accelerate/aabb.h"
#include "Accelerate/aabb.h"
#include "Accelerate/aabb.h"
#include "Accelerate/aabb.h"
#include "Accelerate/aabb.h"
#include "Accelerate/aabb.h"
#include "Accelerate/aabb.h"


std::optional<HitInfo> Scene::intersect(const Ray& ray, float tMin, float tMax) const
{
    return bvh_.intersect(ray, tMin, tMax);
}

void Scene::addInstance(const Shape* shape, Material* material,
                     const glm::vec3& translation,
                     const glm::vec3& scale,
                     const glm::vec3& rotationDeg)
{
    assert(shape && material && "addShape adopts ownership of non-null heap pointers");

    const glm::mat4 model =
        glm::translate(glm::mat4(1.0f), translation) *
        glm::rotate   (glm::mat4(1.0f), glm::radians(rotationDeg.z), glm::vec3{0,0,1}) *
        glm::rotate   (glm::mat4(1.0f), glm::radians(rotationDeg.y), glm::vec3{0,1,0}) *
        glm::rotate   (glm::mat4(1.0f), glm::radians(rotationDeg.x), glm::vec3{1,0,0}) *
        glm::scale    (glm::mat4(1.0f), scale);

    ShapeInstance inst{};
    inst.shape              = shape;
    inst.material           = material;
    inst.modelMatrix        = model;
    inst.inverseModelMatrix = glm::inverse(model);
    inst.updateAABB();

    instances_.push_back(inst);
}




