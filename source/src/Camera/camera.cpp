#include <print>

#include "pch.h"

#include "Camera/camera.h"

Camera::Camera(Film& film, const glm::vec3& eye, const glm::vec3& lookAt, float fovy)
    : film_(film), eye_(eye), fovy_(fovy)
{
    viewDir_ = glm::normalize(lookAt - eye_);
    update();
    theta_ = glm::degrees(glm::acos(viewDir_.y));
    if (glm::abs(viewDir_.y) == 1) {
        phi_ = 0;
    } else {
        phi_ = glm::degrees(glm::acos(viewDir_.x / glm::sqrt(viewDir_.x * viewDir_.x + viewDir_.z * viewDir_.z)));
    }
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

void Camera::move(float dt, Direction direction)
{
    glm::vec3 forward = viewDir_;
    forward.y = 0;
    forward = glm::normalize(forward);
    glm::vec3 moveDir{};

    switch (direction) {
    case Direction::Forward:
        moveDir = forward;
        break;
    case Direction::Backward:
        moveDir = -forward;
        break;
    case Direction::Left:
        moveDir = glm::cross(forward, glm::vec3(0, 1, 0));
        break;
    case Direction::Right:
        moveDir = -glm::cross(forward, glm::vec3(0, 1, 0));
        break;
    case Direction::Up:
        moveDir = glm::vec3(0, 1, 0);
        break;
    case Direction::Down:
        moveDir = glm::vec3(0, -1, 0);
        break;
    }

    eye_ += dt * moveSpeed_ * moveDir;
    update();
}

void Camera::turn(const glm::vec2& delta)
{
    phi_ -= delta.x * turnSpeed_.x;
    if (phi_ > 360)
        phi_ -= 360;
    if (phi_ < 0)
        phi_ += 360;
    theta_ += delta.y * turnSpeed_.y;
    theta_ = glm::clamp(theta_, -179.f, 179.f);

    const float sinTheta = glm::sin(glm::radians(theta_));
    const float cosTheta = glm::cos(glm::radians(theta_));
    const float sinPhi = glm::sin(glm::radians(phi_));
    const float cosPhi = glm::cos(glm::radians(phi_));
    viewDir_ = {sinTheta * cosPhi, cosTheta, sinTheta * sinPhi};
    update();
}

void Camera::zoom(float delta)
{
    fovy_ = glm::clamp(fovy_ - delta, 1.f, 179.f);
    update();
}

void Camera::print()
{
    auto viewpoint = eye_ + viewDir_;
    std::println("Camera:");
    std::println("Film Resolution: ({}, {})", film_.getWidth(), film_.getHeight());
    std::println("Position: ({:f}, {:f}, {:f})", eye_.x, eye_.y, eye_.z);
    std::println("Viewpoint: ({:f}, {:f}, {:f})", viewpoint.x, viewpoint.y, viewpoint.z);
    std::println("Fovy: {:f}", fovy_);
}

void Camera::update()
{
    auto viewpoint = eye_ + viewDir_;
    cameraFromClip_ = glm::inverse(glm::perspective(
        glm::radians(fovy_),
        static_cast<float>(film_.getWidth()) / static_cast<float>(film_.getHeight()), 1.f, 2.f));
    worldFromCamera_ = glm::inverse(glm::lookAt(eye_, viewpoint, {0, 1, 0}));
}
