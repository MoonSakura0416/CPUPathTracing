#include "Camera/ray.h"

Ray Ray::transform(const glm::mat4& invModel) const
{
    glm::vec3 newOrigin = invModel * glm::vec4(origin, 1.0f);
    glm::vec3 newDir = invModel * glm::vec4(direction, 0.0f);
    return {newOrigin, newDir};
}