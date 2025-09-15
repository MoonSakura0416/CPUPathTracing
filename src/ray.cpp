#include "ray.h"

Ray Ray::transform(const glm::mat4& invModel) const
{
    glm::vec4 newOrigin = invModel * glm::vec4(origin, 1.0f);
    glm::vec4 newDir = invModel * glm::vec4(direction, 0.0f);
    return {newOrigin, newDir};
}