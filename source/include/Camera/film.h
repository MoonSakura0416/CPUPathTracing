#pragma once

#include "pch.h"

struct Pixel {
    glm::vec3 color{0, 0, 0};
    int       spp{0};  // samples per pixel
};

class Film {
public:
    Film(size_t width, size_t height);

    /** Saves the film to a PPM file at the specified path.
     *
     * @param path The filesystem path where the PPM file will be saved.
     */
    void save(const std::filesystem::path& path) const;

    [[nodiscard]] size_t getWidth() const noexcept
    {
        return width_;
    }

    [[nodiscard]] size_t getHeight() const noexcept
    {
        return height_;
    }

    [[nodiscard]] Pixel getPixel(size_t x, size_t y) const noexcept
    {
        return pixels_[y * width_ + x];
    }

    void addSample(size_t x, size_t y, const glm::vec3& color)
    {
        if (glm::any(glm::isnan(color))) {
            return;
        }
        pixels_[y * width_ + x].color += color;
        pixels_[y * width_ + x].spp++;
    }

    void clear()
    {
        pixels_.clear();
        pixels_.resize(width_ * height_);
    }

    void setResolution(size_t width, size_t height)
    {
        width_ = width;
        height_ = height;
        pixels_.resize(width_ * height_);
    }

    std::vector<uint8_t> generateRGBABuffer() const;

private:
    size_t             width_{0}, height_{0};
    std::vector<Pixel> pixels_;
};
