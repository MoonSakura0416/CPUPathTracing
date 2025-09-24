#pragma once

#include "pch.h"

struct Complex {
    float real;
    float imag;

    constexpr Complex(float r, float i) : real(r), imag(i) {}
    explicit constexpr Complex(float r) : real(r), imag(0.0f) {}
};

inline constexpr Complex operator+(Complex lhs, Complex rhs) {
    return { lhs.real + rhs.real, lhs.imag + rhs.imag };
}

inline constexpr Complex operator-(Complex lhs, Complex rhs) {
    return { lhs.real - rhs.real, lhs.imag - rhs.imag };
}

inline constexpr Complex operator*(Complex lhs, Complex rhs) {
    return {
        lhs.real * rhs.real - lhs.imag * rhs.imag,
        lhs.real * rhs.imag + lhs.imag * rhs.real
    };
}

inline constexpr Complex operator*(Complex lhs, float rhs) {
    return { lhs.real * rhs, lhs.imag * rhs };
}

inline constexpr Complex operator/(Complex lhs, Complex rhs) {
    float denom = rhs.real * rhs.real + rhs.imag * rhs.imag;
    if (denom == 0.0f) {
        throw std::invalid_argument("Complex division by 0");
    }
    float invDenom = 1.f / denom;
    return {
        (lhs.real * rhs.real + lhs.imag * rhs.imag) * invDenom,
        (lhs.imag * rhs.real - lhs.real * rhs.imag) * invDenom
    };
}

inline constexpr Complex operator/(Complex lhs, float rhs) {
    return { lhs.real / rhs, lhs.imag / rhs };
}

inline constexpr float norm(Complex z) {
    return (z.real * z.real + z.imag * z.imag);
}

inline Complex sqrt(Complex z) {
    const float r = norm(z);
    float real = std::sqrt((r + z.real) * 0.5f);
    float imag = std::copysign(std::sqrt((r - z.real) * 0.5f), z.imag);
    return { real, imag };
}
