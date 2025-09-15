#pragma once

#include <limits>
#include <numbers>

constexpr float Infinity = std::numeric_limits<float>::infinity();
constexpr float Epsilon = 1e-5f;
constexpr float Pi = std::numbers::pi_v<float>;
