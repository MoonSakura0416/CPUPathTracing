#pragma once

#include <glm/glm.hpp>

struct RGB {
    constexpr RGB(int r, int g, int b) : r(r), g(g), b(b) {}

    explicit RGB(const glm::vec3& color)
    {
        r = glm::clamp<int>(glm::pow(color.x, 1.0 / 2.2) * 255.0f, 0, 255);
        g = glm::clamp<int>(glm::pow(color.y, 1.0 / 2.2) * 255.0f, 0, 255);
        b = glm::clamp<int>(glm::pow(color.z, 1.0 / 2.2) * 255.0f, 0, 255);
    }



    operator glm::vec3() const
    {
        return {glm::pow(r / 255.f, 2.2f), glm::pow(g / 255.f, 2.2f), glm::pow(b / 255.f, 2.2f)};
    }

    int r, g, b;
};