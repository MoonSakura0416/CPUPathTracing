#include "pch.h"

#include "Renderer/base_renderer.h"
#include "Thread/thread_pool.h"
#include "Util/progress.h"
#include "Util/profile.h"



void BaseRenderer::render(size_t spp, const std::filesystem::path& path)
{
    PROFILE("Render " + std::to_string(spp) + " spp to " + path.string())
    size_t   currentSpp = 0, step = 1;
    auto&    film = camera_.getFilm();
    Progress progress{film.getWidth() * film.getHeight() * spp};
    while (currentSpp < spp) {
        threadpool.parallelFor(
            film.getWidth(), film.getHeight(),
            [&, currentSpp, baseSeed = 1337u](size_t x, size_t y) {
                auto mix = [](uint64_t v) {
                    v += 0x9e3779b97f4a7c15ull;
                    v = (v ^ (v >> 30)) * 0xbf58476d1ce4e5b9ull;
                    v = (v ^ (v >> 27)) * 0x94d049bb133111ebull;
                    return v ^ (v >> 31);
                };
                uint64_t seed = mix(baseSeed) ^ mix(x) ^ mix(y) ^ mix(currentSpp);
                RNG      rng(static_cast<size_t>(seed));
                for (size_t i = 0; i < step; i++) {
                    film.addSample(x, y, renderPixel({x, y}, rng));
                }
                progress.update(step);
            });
        threadpool.wait();

        currentSpp += step;
        size_t remaining = spp - currentSpp;
        step = remaining > step ? std::min<size_t>(currentSpp, 32) : remaining;

        film.save(path);
        std::println("{} spp has been saved to {}", currentSpp, path.string());
    }
}
