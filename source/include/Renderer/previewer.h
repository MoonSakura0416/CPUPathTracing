#pragma once

#include "base_renderer.h"

class Previewer {
public:
    explicit Previewer(BaseRenderer& baseRenderer, float fps = 30);
    bool preview();

private:
    void renderFrame();
    void setResolution(float scale);
    void adjustResolution(float dt);

private:
    BaseRenderer&                              baseRenderer_;
    std::vector<std::shared_ptr<BaseRenderer>> renderModes_;
    size_t                                     renderModeIdx_{0};

    float      scale_{1};
    float      fps_;
    glm::ivec2 filmResolution_{};

    size_t currentSpp_{0};

    std::shared_ptr<sf::RenderWindow> window_;
    std::shared_ptr<sf::Texture>      texture_;
    std::shared_ptr<sf::Sprite>       sprite_;
};
