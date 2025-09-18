#include "pch.h"

#include "Camera/camera.h"



Camera::Camera(Film& film, const glm::vec3& eye, const glm::vec3& lookAt, float fovy)
    :film_(film), eye_(eye)
{
    cameraFromClip_ = glm::inverse(glm::perspective(
        glm::radians(fovy), static_cast<float>(film_.getWidth()) / film_.getHeight(), 1.f, 2.f));
    worldFromCamera_= glm::inverse(glm::lookAt(eye, lookAt, {0,1,0}));
}

Ray Camera::getRay(const glm::ivec2& pixelCoord, const glm::vec2& offset) const
{
    glm::vec2 ndc =
        (glm::vec2(pixelCoord) + offset) / glm::vec2(film_.getWidth(), film_.getHeight());
    ndc.y = 1 - ndc.y;
    ndc = 2.f * ndc - 1.f;
    glm::vec4 clip{ndc, 0, 1};
    glm::vec3 world = worldFromCamera_ * cameraFromClip_ * clip;
    return {eye_, glm::normalize(world - eye_)};
}
