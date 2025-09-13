#include "film.h"

#include <fstream>

Film::Film(size_t width, size_t height) : width_(width), height_(height)
{
    pixels_.resize(width * height);
}

void Film::Save(const std::filesystem::path& path) const
{
    std::ofstream file(path);
    file << "P6\n" << width_ << ' ' << height_ << "\n255\n";
    for (size_t j = 0; j < height_; ++j) {
        for (size_t i = 0; i < width_; ++i) {
            const auto&      color = GetPixel(i, j);
            const glm::ivec3 colorInt = glm::clamp(color * 255.f, 0.f, 255.f);
            file << static_cast<uint8_t>(colorInt.x) << " "
                 << static_cast<uint8_t>(colorInt.y) << " "
                 << static_cast<uint8_t>(colorInt.z) << "\n";
        }
    }
}
