#pragma once

#include <assert.h>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <string.h>
#include <vector>

namespace PBRender {

// Global Constants
static float MachineEpsilon = std::numeric_limits<float>::epsilon() * 0.5;

static float Infinity = std::numeric_limits<float>::infinity();

static float Pi = 3.14159265358979323846;
static float InvPi = 0.31830988618379067154;
static float Inv2Pi = 0.15915494309189533577;
static float Inv4Pi = 0.07957747154594766788;
static float PiOver2 = 1.57079632679489661923;
static float PiOver4 = 0.78539816339744830961;
static float Sqrt2 = 1.41421356237309504880;

// Geometry
template <typename T>
class Vector2;

template <typename T>
class Vector3;

template <typename T>
class Point3;

template <typename T>
class Point2;

template <typename T>
class Normal3;

template <typename T>
class Bounds2;

template <typename T>
class Bounds3;

class Ray;
class Transform;
struct Interaction;
class SurfaceInteraction;
class Shape;
class Primitive;

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

inline float gamma(int n) {
    return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
}

template <typename T, typename U, typename V>
inline T Clamp(T val, U low, V high) {
    if (val < low)
        return low;
    else if (val > high)
        return high;
    else
        return val;
}

template <typename T>
inline T Mod(T a, T b) {
    T result = a - (a / b) * b;
    return (T)((result < 0) ? result + b : result);
}

template <>
inline float Mod(float a, float b) {
    return std::fmod(a, b);
}

inline float Radians(float deg) { return (Pi / 180) * deg; }

inline float Degrees(float rad) { return (180 / Pi) * rad; }

inline float Log2(float x) {
    const float invLog2 = 1.442695040888963387004650940071;
    return std::log(x) * invLog2;
}

inline float Lerp(float t, float v1, float v2) { return (1 - t) * v1 + t * v2; }
}