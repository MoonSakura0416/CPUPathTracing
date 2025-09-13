#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <filesystem>

class Film {
public:
    Film(size_t width, size_t height);

    /** Saves the film to a PPM file at the specified path.
     *
     * @param path The filesystem path where the PPM file will be saved.
     */
    void Save(const std::filesystem::path& path) const;

    [[nodiscard]] size_t GetWidth() const
    {
        return width_;
    }

    [[nodiscard]] size_t GetHeight() const
    {
        return height_;
    }

    [[nodiscard]] glm::vec3 GetPixel(size_t x, size_t y) const
    {
        return pixels_[y * width_ + x];
    }

    void SetPixel(size_t x, size_t y, const glm::vec3& color)
    {
        pixels_[y * width_ + x] = color;
    }

private:
    size_t                 width_{0}, height_{0};
    std::vector<glm::vec3> pixels_;
};
