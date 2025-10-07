#include "Renderer/previewer.h"
#include "Renderer/normal_renderer.h"
#include "Renderer/debug_renderer.h"
#include "Thread/thread_pool.h"

Previewer::Previewer(BaseRenderer& baseRenderer, float fps) : baseRenderer_(baseRenderer), fps_(fps)
{
    auto& film = baseRenderer_.camera_.getFilm();
    filmResolution_ = {film.getWidth(), film.getHeight()};

    renderModes_.push_back(std::shared_ptr<BaseRenderer>(&baseRenderer_, [](BaseRenderer*) {}));

    renderModes_.push_back(
        std::make_shared<NormalRenderer>(baseRenderer_.camera_, baseRenderer_.scene_));

    DEBUG_LINE(renderModes_.push_back(
        std::make_shared<AABBTestCountRenderer>(baseRenderer_.camera_, baseRenderer_.scene_)))

    DEBUG_LINE(renderModes_.push_back(
        std::make_shared<TriTestCountRenderer>(baseRenderer_.camera_, baseRenderer_.scene_)))
}

bool Previewer::preview()
{
    window_ = std::make_shared<sf::RenderWindow>(
        sf::VideoMode(sf::Vector2u(filmResolution_.x, filmResolution_.y)), "Previewer",
        sf::Style::None, sf::State::Windowed);
    texture_ = std::make_shared<sf::Texture>(sf::Vector2u(filmResolution_.x, filmResolution_.y));
    texture_->setSmooth(true);
    sprite_ = std::make_shared<sf::Sprite>(*texture_);
    setResolution(0.1);
    auto& camera = baseRenderer_.camera_;
    auto& film = camera.getFilm();

    bool               grabbed = false;
    sf::Vector2i center{window_->getSize() / 2u};
    float              dt = 0;
    bool               result = false;

    using S = sf::Keyboard::Scancode;

    while (window_->isOpen()) {
        while (auto event = window_->pollEvent()) {
            if (auto* key_released = event->getIf<sf::Event::KeyReleased>()) {
                if (key_released->scancode == S::Escape) {
                    window_->close();
                } else if (key_released->scancode == S::Enter) {
                    window_->close();
                    result = true;
                } else if (key_released->scancode == S::Tab) {
                    renderModeIdx_ = (renderModeIdx_ + 1) % renderModes_.size();
                    currentSpp_ = 0;
                } else if (key_released->scancode == S::Equal) {
                    fps_ += 1;
                    std::println("FPS: {:f}", fps_);
                } else if (key_released->scancode == S::Hyphen) {
                    fps_ -= 1;
                    if (fps_ < 1) {
                        fps_ = 1;
                    }
                    std::println("FPS: {:f}", fps_);
                } else if (key_released->scancode == S::CapsLock) {
                    grabbed = !grabbed;
                    window_->setMouseCursorGrabbed(grabbed);
                    window_->setMouseCursorVisible(!grabbed);
                    sf::Mouse::setPosition(center, *window_);
                }
            } else if (auto* mouse_moved = event->getIf<sf::Event::MouseMoved>()) {
                if (!grabbed) {
                    continue;
                }
                auto delta = mouse_moved->position - center;
                if (delta.x == 0 && delta.y == 0) {
                    continue;
                }
                camera.turn({delta.x, delta.y});
                currentSpp_ = 0;
                sf::Mouse::setPosition(center, *window_);
            } else if (auto* mouse_wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
                if (!grabbed) {
                    continue;
                }
                camera.zoom(mouse_wheel->delta);
                currentSpp_ = 0;
            }
        }

        if (grabbed) {
            if (sf::Keyboard::isKeyPressed(S::W)) {
                camera.move(dt, Direction::Forward);
                currentSpp_ = 0;
            }
            if (sf::Keyboard::isKeyPressed(S::S)) {
                camera.move(dt, Direction::Backward);
                currentSpp_ = 0;
            }
            if (sf::Keyboard::isKeyPressed(S::A)) {
                camera.move(dt, Direction::Left);
                currentSpp_ = 0;
            }
            if (sf::Keyboard::isKeyPressed(S::D)) {
                camera.move(dt, Direction::Right);
                currentSpp_ = 0;
            }
            if (sf::Keyboard::isKeyPressed(S::Space)) {
                camera.move(dt, Direction::Up);
                currentSpp_ = 0;
            }
            if (sf::Keyboard::isKeyPressed(S::LShift)) {
                camera.move(dt, Direction::Down);
                currentSpp_ = 0;
            }
        }

        auto start = std::chrono::high_resolution_clock::now();
        renderFrame();
        auto duration = std::chrono::high_resolution_clock::now() - start;

        auto buffer = film.generateRGBABuffer();
        texture_->update(buffer.data());

        window_->clear();
        window_->draw(*sprite_);
        window_->display();

        dt = std::chrono::duration<float>(duration).count();
        if (dt < 1e-6f) dt = 1e-6f;
        adjustResolution(dt);
    }

    film.setResolution(filmResolution_.x, filmResolution_.y);
    camera.print();
    return result;
}

void Previewer::renderFrame()
{
    const auto   renderer = renderModes_[renderModeIdx_].get();
    const size_t renderSpp = renderModeIdx_ == 0 ? 4 : 1;
    auto&  film = renderer->camera_.getFilm();

    if (currentSpp_ == 0) {
        film.clear();
    }

    threadpool.parallelFor(
        film.getWidth(), film.getHeight(), [&, baseSeed = 19990416u](size_t x, size_t y) {
            auto mix = [](uint64_t v) {
                v += 0x9e3779b97f4a7c15ull;
                v = (v ^ (v >> 30)) * 0xbf58476d1ce4e5b9ull;
                v = (v ^ (v >> 27)) * 0x94d049bb133111ebull;
                return v ^ (v >> 31);
            };
            const uint64_t seed = mix(baseSeed) ^ mix(x) ^ mix(y) ^ mix(currentSpp_);
            RNG            rng(seed);
            for (size_t i = currentSpp_; i < currentSpp_ + renderSpp; i++) {
                film.addSample(x, y, renderer->renderPixel({x, y}, rng));
            }
        });
    threadpool.wait();
    currentSpp_ += renderSpp;
}

void Previewer::setResolution(float scale)
{
    scale = glm::clamp(scale, 0.f, 1.f);
    if (scale_ == scale) return;
    scale_ = scale;

    glm::ivec2 resolution = glm::max(glm::ivec2(1), glm::ivec2(scale * glm::vec2(filmResolution_)));

    for (auto& r : renderModes_) {
        r->camera_.getFilm().setResolution(resolution.x, resolution.y);
    }

    auto res = texture_->resize(sf::Vector2u(resolution.x, resolution.y));
    sprite_->setTexture(*texture_, true);
    sprite_->setScale(sf::Vector2f(
        static_cast<float>(filmResolution_.x) / resolution.x,
        static_cast<float>(filmResolution_.y) / resolution.y
    ));

    currentSpp_ = 0;
}

void Previewer::adjustResolution(float dt)
{
    if (dt < 1e-6f) return;
    float expected_dt = 1.f / fps_;
    if (glm::abs(expected_dt - dt) / expected_dt > 0.33f) {
        const float newScale = scale_ * (1.f + 0.1f * (glm::sqrt(expected_dt / dt) - 1.f));
        setResolution(newScale);
    }
}
