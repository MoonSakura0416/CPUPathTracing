#pragma once

#include "pch.h"

#define PROFILE(name) Profile __profile{name};

struct Profile {
    explicit Profile(std::string name);
    ~Profile();
    std::string                                    name;
    std::chrono::high_resolution_clock::time_point start;
};