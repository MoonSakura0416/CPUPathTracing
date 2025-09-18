#pragma once

#include "pch.h"

class RGB Lerp(RGB a, RGB b, float t);

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
        //if (t < 0.0f || t > 1.0f) { return {255, 0, 0}; }

        // std::array<RGB, 25> colorPallet{
        //     RGB{68, 1, 84},    RGB{71, 17, 100},  RGB{72, 31, 112},
        //     RGB{71, 45, 123},  RGB{68, 58, 131},
        //
        //     RGB{64, 70, 136},  RGB{59, 82, 139},  RGB{54, 93, 141},
        //     RGB{49, 104, 142}, RGB{44, 114, 142},
        //
        //     RGB{40, 124, 142}, RGB{36, 134, 142}, RGB{33, 144, 140},
        //     RGB{31, 154, 138}, RGB{32, 164, 134},
        //
        //     RGB{39, 173, 129}, RGB{53, 183, 121}, RGB{71, 193, 110},
        //     RGB{93, 200, 99},  RGB{117, 208, 84},
        //
        //     RGB{143, 215, 68}, RGB{170, 220, 50}, RGB{199, 224, 32},
        //     RGB{227, 228, 24}, RGB{253, 231, 37},
        // };

        std::array<RGB, 25> colorPallet{
            RGB{  0,   0, 128},  // Deep Blue
            RGB{  0,  32, 160},
            RGB{  0,  64, 192},
            RGB{  0,  96, 224},
            RGB{  0, 128, 255},  // Bright Blue

            RGB{ 32, 160, 224},
            RGB{ 64, 192, 192},
            RGB{ 96, 208, 160},
            RGB{128, 224, 128},
            RGB{160, 240, 96},   // Green-Yellow transition

            RGB{192, 255, 64},
            RGB{208, 240, 48},
            RGB{224, 224, 32},
            RGB{240, 208, 24},
            RGB{255, 192, 16},   // Bright Yellow

            RGB{255, 160,  32},
            RGB{255, 128,  48},
            RGB{255,  96,  64},
            RGB{255,  64,  80},
            RGB{255,  32,  96},  // Orange Red

            RGB{255,   0, 112},
            RGB{240,   0,  96},
            RGB{224,   0,  80},
            RGB{208,   0,  64},
            RGB{192,   0,  48},  // Crimson Red
        };

        if (t == 1.0f) {
            return colorPallet.back();
        }

        // Set index in [0, size-1)
        float indexFloat = t * (colorPallet.size() - 1);
        auto index = static_cast<size_t>(glm::floor(indexFloat));
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