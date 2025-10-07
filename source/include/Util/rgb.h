#pragma once

#include "pch.h"

struct RGB Lerp(RGB a, RGB b, float t);

struct RGB {
    constexpr RGB(int r, int g, int b) : r(r), g(g), b(b) {}

    explicit RGB(const glm::vec3& color)
    {
        r = glm::clamp<int>(glm::pow(color.x, 1.0 / 2.2) * 255.0f, 0, 255);
        g = glm::clamp<int>(glm::pow(color.y, 1.0 / 2.2) * 255.0f, 0, 255);
        b = glm::clamp<int>(glm::pow(color.z, 1.0 / 2.2) * 255.0f, 0, 255);
    }

    static RGB GenerateHeatMapRGB(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);

        std::array<RGB, 25> colorPallet{
            RGB{0, 0, 128},  // Deep Blue
            RGB{0, 32, 160},    RGB{0, 64, 192},   RGB{0, 96, 224},
            RGB{0, 128, 255},  // Bright Blue

            RGB{32, 160, 224},  RGB{64, 192, 192}, RGB{96, 208, 160},
            RGB{128, 224, 128}, RGB{160, 240, 96},  // Green-Yellow transition

            RGB{192, 255, 64},  RGB{208, 240, 48}, RGB{224, 224, 32},
            RGB{240, 208, 24},  RGB{255, 192, 16},  // Bright Yellow

            RGB{255, 160, 32},  RGB{255, 128, 48}, RGB{255, 96, 64},
            RGB{255, 64, 80},   RGB{255, 32, 96},  // Orange Red

            RGB{255, 0, 112},   RGB{240, 0, 96},   RGB{224, 0, 80},
            RGB{208, 0, 64},    RGB{192, 0, 48},  // Crimson Red
        };

        if (t == 1.0f) {
            return colorPallet.back();
        }

        // Set index in [0, size-1)
        float indexFloat = t * (colorPallet.size() - 1);
        auto  index = static_cast<size_t>(glm::floor(indexFloat));
        float fraction = glm::fract(indexFloat);

        return Lerp(colorPallet[index], colorPallet[index + 1], fraction);
    }

    operator glm::vec3() const
    {
        return {glm::pow(r / 255.f, 2.2f), glm::pow(g / 255.f, 2.2f), glm::pow(b / 255.f, 2.2f)};
    }

    int r, g, b;
};

inline RGB Lerp(RGB a, RGB b, float t)
{
    return {glm::clamp<int>(a.r + (b.r - a.r) * t, 0, 255),
            glm::clamp<int>(a.g + (b.g - a.g) * t, 0, 255),
            glm::clamp<int>(a.b + (b.b - a.b) * t, 0, 255)};
}