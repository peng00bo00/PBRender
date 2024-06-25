#pragma once

#include <assert.h>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <string.h>
#include <vector>

#include <iostream>
#include <sstream>

namespace PBRender
{

// Vector
template <typename T>
struct Vector2;
template <typename T>
struct Vector3;
template <typename T>
struct Point3;
template <typename T>
struct Point2;
template <typename T>
struct Normal3;

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;
using Vector3f = Vector3<float>;
using Vector3i = Vector3<int>;

using Point2f  = Point2<float>;
using Point2i  = Point2<int>;
using Point3f  = Point3<float>;

// Normal
template <typename T>
struct Normal3;

using Normal3f = Normal3<float>;

// Bounds
template <typename T>
class Bounds2;
// template <typename T>
// class Bounds3;

using Bounds2f = Bounds2<float>;
using Bounds2i = Bounds2<int>;

// using Bounds3f = Bounds3<float>;
// using Bounds3i = Bounds3<int>;

// Scene
class Scene;

// Engine
class Engine;

// Transform
class Transform;

// Camera
class Camera;
struct CameraSample;
class ProjectiveCamera;

// Film
class Film;

// Ray
class Ray;

// Global Inline Functions
inline uint32_t FloatToBits(float f) {
    uint32_t ui;
    memcpy(&ui, &f, sizeof(float));
    return ui;
}

inline float BitsToFloat(uint32_t ui) {
    float f;
    memcpy(&f, &ui, sizeof(uint32_t));
    return f;
}

inline uint64_t FloatToBits(double f) {
    uint64_t ui;
    memcpy(&ui, &f, sizeof(double));
    return ui;
}

inline double BitsToFloat(uint64_t ui) {
    double f;
    memcpy(&f, &ui, sizeof(uint64_t));
    return f;
}

inline float NextFloatUp(float v) {
    // Handle infinity and negative zero for _NextFloatUp()_
    if (std::isinf(v) && v > 0.) return v;
    if (v == -0.f) v = 0.f;

    // Advance _v_ to next higher float
    uint32_t ui = FloatToBits(v);
    if (v >= 0)
        ++ui;
    else
        --ui;
    return BitsToFloat(ui);
}

inline float NextFloatDown(float v) {
    // Handle infinity and positive zero for _NextFloatDown()_
    if (std::isinf(v) && v < 0.) return v;
    if (v == 0.f) v = -0.f;
    uint32_t ui = FloatToBits(v);
    if (v > 0)
        --ui;
    else
        ++ui;
    return BitsToFloat(ui);
}

inline double NextFloatUp(double v, int delta = 1) {
    if (std::isinf(v) && v > 0.) return v;
    if (v == -0.f) v = 0.f;
    uint64_t ui = FloatToBits(v);
    if (v >= 0.)
        ui += delta;
    else
        ui -= delta;
    return BitsToFloat(ui);
}

inline double NextFloatDown(double v, int delta = 1) {
    if (std::isinf(v) && v < 0.) return v;
    if (v == 0.f) v = -0.f;
    uint64_t ui = FloatToBits(v);
    if (v > 0.)
        ui -= delta;
    else
        ui += delta;
    return BitsToFloat(ui);
}

} // namespace PBRender
