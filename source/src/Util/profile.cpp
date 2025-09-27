#include "pch.h"

#include "Util/profile.h"

Profile::Profile(std::string name)
    : name(std::move(name)), start(std::chrono::high_resolution_clock::now())
{
}

Profile::~Profile()
{
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = end - start;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
    std::println("Profile: {} took {} ms", name, ms);
}