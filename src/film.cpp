#include "film.h"
#include "rgb.h"

#include <fstream>

Film::Film(size_t width, size_t height) : width_(width), height_(height)
{
    if (width <= 0 || height <= 0)
        throw std::invalid_argument("Film size must be positive");
    pixels_.resize(width * height);
}

void Film::save(const std::filesystem::path& path) const
{
    std::ofstream file(path, std::ios::binary);
    file << "P6\n" << width_ << ' ' << height_ << "\n255\n";
    for (size_t j = 0; j < height_; ++j) {
        for (size_t i = 0; i < width_; ++i) {
            RGB rgb{getPixel(i,j)};
            file << rgb.r << rgb.g << rgb.b;
        }
    }
}
