cmake_minimum_required(VERSION 3.15...4.0)
project(raytracer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
endif ()

# ========== Option ==========
option(WARNINGS_AS_ERRORS "Treat warnings as errors" ON)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(ENABLE_LTO "Enable Link Time Optimization (IPO)" OFF)

# ========== Dependence ==========
# The system needs to be installed：glm、assimp
find_package(glm REQUIRED CONFIG)
#find_package(assimp REQUIRED)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(raytracer main.cpp ${APP_SOURCES})

target_link_libraries(raytracer PRIVATE
        glm::glm
        #assimp::assimp
)

target_include_directories(raytracer PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


# ========== Warning (cross-platform） ==========
if (MSVC)
    target_compile_options(raytracer PRIVATE
            /W4
            $<$<BOOL:${WARNINGS_AS_ERRORS}>:/WX>
    )
else ()
    target_compile_options(raytracer PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion
            $<$<BOOL:${WARNINGS_AS_ERRORS}>:-Werror>
    )
endif ()

# ========== Sanitizers ==========
# Clang/GCC: ASan/UBSan
if (NOT MSVC)
    if (ENABLE_ASAN)
        target_compile_options(raytracer PRIVATE -fsanitize=address)
        target_link_libraries(raytracer PRIVATE -fsanitize=address)
    endif ()
    if (ENABLE_UBSAN)
        target_compile_options(raytracer PRIVATE -fsanitize=undefined)
        target_link_libraries(raytracer PRIVATE -fsanitize=undefined)
    endif ()
else ()
    if (ENABLE_ASAN)
        target_compile_options(raytracer PRIVATE /fsanitize=address)
        target_link_options(raytracer PRIVATE /fsanitize=address)
    endif ()
endif ()

# ========== LTO/IPO ==========
if (ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT lto_ok OUTPUT lto_msg)
    if (lto_ok)
        set_property(TARGET raytracer PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else ()
        message(WARNING "IPO/LTO not supported: ${lto_msg}")
    endif ()
endif ()