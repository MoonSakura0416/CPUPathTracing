#include "Camera/film.h"
#include "Util/rgb.h"
#include "Util/profile.h"
#include "Thread/thread_pool.h"

#include <fstream>

Film::Film(size_t width, size_t height) : width_(width), height_(height)
{
    if (width <= 0 || height <= 0)
        throw std::invalid_argument("Film size must be positive");
    pixels_.resize(width * height);
}

void Film::save(const std::filesystem::path& path) const
{
    PROFILE("Save to " + path.string())
    std::ofstream file(path, std::ios::binary);
    file << "P6\n" << width_ << ' ' << height_ << "\n255\n";

    std::vector<uint8_t> buffer(width_ * height_ * 3);
    threadpool.parallelFor(
        width_, height_,
        [&](size_t x, size_t y) {
            auto [color, spp] = getPixel(x, y);
            RGB    rgb{color / static_cast<float>(spp)};
            size_t index = (y * width_ + x) * 3;
            buffer[index] = static_cast<uint8_t>(rgb.r);
            buffer[index + 1] = static_cast<uint8_t>(rgb.g);
            buffer[index + 2] = static_cast<uint8_t>(rgb.b);
        },
        false);
    threadpool.wait();

    file.write(reinterpret_cast<const char*>(buffer.data()),
               static_cast<std::streamsize>(buffer.size()));
}
