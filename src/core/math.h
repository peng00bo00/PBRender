#pragma once

#include "common.h"

namespace PBRender
{

template <typename T>
inline bool isNaN(const T x) {
    return std::isnan(x);
}

template <>
inline bool isNaN(const int x) {
    return false;
}

// Mathematical Constants
static constexpr float MachineEpsilon = std::numeric_limits<float>::epsilon() * 0.5;

static constexpr float Infinity = std::numeric_limits<float>::infinity();

static constexpr float ShadowEpsilon = 0.0001f;
static constexpr float Pi            = 3.14159265358979323846;
static constexpr float InvPi         = 0.31830988618379067154;
static constexpr float Inv2Pi        = 0.15915494309189533577;
static constexpr float Inv4Pi        = 0.07957747154594766788;
static constexpr float PiOver2       = 1.57079632679489661923;
static constexpr float PiOver4       = 0.78539816339744830961;
static constexpr float Sqrt2         = 1.41421356237309504880;

// useful functions
inline float gamma(int n) {
    return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
}

inline float GammaCorrect(float value) {
    if (value <= 0.0031308f) return 12.92f * value;
    return 1.055f * std::pow(value, (float)(1.f / 2.4f)) - 0.055f;
}

inline float InverseGammaCorrect(float value) {
    if (value <= 0.04045f) return value * 1.f / 12.92f;
    return std::pow((value + 0.055f) * 1.f / 1.055f, (float)2.4f);
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

inline int Log2Int(uint32_t v) {
    return 31 - __builtin_clz(v);
}

inline int Log2Int(int32_t v) { return Log2Int((uint32_t)v); }

inline int Log2Int(uint64_t v) {
    return 63 - __builtin_clzll(v);
}

inline int Log2Int(int64_t v) { return Log2Int((uint64_t)v); }

template <typename T>
inline constexpr bool IsPowerOf2(T v) {
    return v && !(v & (v - 1));
}

inline int32_t RoundUpPow2(int32_t v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    return v + 1;
}

inline int64_t RoundUpPow2(int64_t v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    return v + 1;
}

inline float Lerp(float t, float v1, float v2) { return (1 - t) * v1 + t * v2; }

inline bool Quadratic(float a, float b, float c, float *t0, float *t1) {
    // Find quadratic discriminant
    double discrim = (double)b * (double)b - 4 * (double)a * (double)c;
    if (discrim < 0) return false;
    double rootDiscrim = std::sqrt(discrim);

    // Compute quadratic _t_ values
    double q;
    if (b < 0)
        q = -.5 * (b - rootDiscrim);
    else
        q = -.5 * (b + rootDiscrim);
    *t0 = q / a;
    *t1 = c / q;
    if (*t0 > *t1) std::swap(*t0, *t1);
    return true;
}

} // namespace PBRender