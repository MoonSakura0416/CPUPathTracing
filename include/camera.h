#pragma once

#include "film.h"
#include "ray.h"

class Camera {
public:
    Camera(Film& film, const glm::vec3& eye, const glm::vec3& lookAt, float fovy);

    [[nodiscard]] Ray getRay(const glm::ivec2& pixelCoord, const glm::vec2& offset = {0.5,0.5}) const;

    [[nodiscard]] Film& getFilm()
    {
        return film_;
    }

    [[nodiscard]] const Film& getFilm() const
    {
        return film_;
    }

private:
    Film& film_;
    glm::vec3 eye_;
    glm::mat4 cameraFromClip_;
    glm::mat4 worldFromCamera_;
};