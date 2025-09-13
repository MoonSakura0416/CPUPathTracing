#include "film.h"

#include <fstream>

Film::Film(size_t width, size_t height) : width_(width), height_(height)
{
    pixels_.resize(width * height);
}

void Film::save(const std::filesystem::path& path) const
{
    std::ofstream file(path, std::ios::binary);
    file << "P6\n" << width_ << ' ' << height_ << "\n255\n";
    for (size_t j = 0; j < height_; ++j) {
        for (size_t i = 0; i < width_; ++i) {
            const auto&       color = getPixel(i, j);
            const glm::u8vec3 colorInt = glm::clamp(color * 255.f, 0.f, 255.f);
            file << colorInt.x << colorInt.y << colorInt.z;
        }
    }
}
