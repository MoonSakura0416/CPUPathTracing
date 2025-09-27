#pragma once

#include "pch.h"

#include "film.h"
#include "ray.h"

enum class Direction : uint8_t {
    Forward,
    Backward,
    Left,
    Right,
    Up,
    Down,
};

class Camera {
public:
    Camera(Film& film, const glm::vec3& eye, const glm::vec3& lookAt, float fovy);

    [[nodiscard]] Ray getRay(const glm::ivec2& pixelCoord,
                             const glm::vec2&  offset = {0.5, 0.5}) const;

    [[nodiscard]] Film& getFilm()
    {
        return film_;
    }

    [[nodiscard]] const Film& getFilm() const
    {
        return film_;
    }

    void move(float dt, Direction direction);

    void turn(const glm::vec2& delta);

    void zoom(float delta);

    void print();

private:
    void update();

private:
    Film&     film_;
    glm::vec3 eye_;
    glm::mat4 cameraFromClip_;
    glm::mat4 worldFromCamera_;

    float     fovy_;
    glm::vec3 viewDir_;
    ;
    float     theta_, phi_;
    float     moveSpeed_{2};
    glm::vec2 turnSpeed_{0.15, 0.1};
};