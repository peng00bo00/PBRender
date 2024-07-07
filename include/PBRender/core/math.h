#pragma once

#include <PBRender/core/common.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>

namespace PBRender
{

template <typename T>
struct complex {
    complex(T re) : re(re), im(0) {}
    complex(T re, T im) : re(re), im(im) {}

    complex operator-() const { return {-re, -im}; }

    complex operator+(complex z) const { return {re + z.re, im + z.im}; }

    complex operator-(complex z) const { return {re - z.re, im - z.im}; }

    complex operator*(complex z) const {
        return {re * z.re - im * z.im, re * z.im + im * z.re};
    }

    complex operator/(complex z) const {
        T scale = 1 / (z.re * z.re + z.im * z.im);
        return {scale * (re * z.re + im * z.im), scale * (im * z.re - re * z.im)};
    }

    friend complex operator+(T value, complex z) {
        return complex(value) + z;
    }

    friend complex operator-(T value, complex z) {
        return complex(value) - z;
    }

    friend complex operator*(T value, complex z) {
        return complex(value) * z;
    }

    friend complex operator/(T value, complex z) {
        return complex(value) / z;
    }

    T re, im;
};

inline float sqrt(float f) {
    return std::sqrt(f);
}
inline double sqrt(double f) {
    return std::sqrt(f);
}
inline float abs(float f) {
    return std::abs(f);
}
inline double abs(double f) {
    return std::fabs(f);
}

inline float copysign(float mag, float sign) {
    return std::copysign(mag, sign);
}

inline double copysign(double mag, double sign) {
    return std::copysign(mag, sign);
}

inline float floor(float arg) {
    return std::floor(arg);
}

inline double floor(double arg) {
    return std::floor(arg);
}

inline float ceil(float arg) {
    return std::ceil(arg);
}

inline double ceil(double arg) {
    return std::ceil(arg);
}

inline float round(float arg) {
    return std::round(arg);
}

inline double round(double arg) {
    return std::round(arg);
}

inline float fmod(float x, float y) {
    return std::fmod(x, y);
}

inline double fmod(double x, double y) {
    return std::fmod(x, y);
}

template <typename T>
T real(const complex<T> &z) {
    return z.re;
}

template <typename T>
T imag(const complex<T> &z) {
    return z.im;
}

template <typename T>
T norm(const complex<T> &z) {
    return z.re * z.re + z.im * z.im;
}

template <typename T>
T abs(const complex<T> &z) {
    return sqrt(norm(z));
}

template <typename T>
complex<T> sqrt(const complex<T> &z) {
    T n = abs(z), t1 = sqrt(T(.5) * (n + abs(z.re))),
      t2 = T(.5) * z.im / t1;

    if (n == 0)
        return 0;

    if (z.re >= 0)
        return {t1, t2};
    else
        return {abs(t2), copysign(t1, z.im)};
}

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

// Bit Operation Inline Functions
inline uint32_t ReverseBits32(uint32_t n) {
    n = (n << 16) | (n >> 16);
    n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
    n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
    n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
    n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
    return n;
}

inline uint64_t ReverseBits64(uint64_t n) {
    uint64_t n0 = ReverseBits32((uint32_t)n);
    uint64_t n1 = ReverseBits32((uint32_t)(n >> 32));
    return (n0 << 32) | n1;
}

// https://fgiesen.wordpress.com/2009/12/13/decoding-morton-codes/
// updated to 64 bits.
inline uint64_t LeftShift2(uint64_t x) {
    x &= 0xffffffff;
    x = (x ^ (x << 16)) & 0x0000ffff0000ffff;
    x = (x ^ (x << 8)) & 0x00ff00ff00ff00ff;
    x = (x ^ (x << 4)) & 0x0f0f0f0f0f0f0f0f;
    x = (x ^ (x << 2)) & 0x3333333333333333;
    x = (x ^ (x << 1)) & 0x5555555555555555;
    return x;
}

inline uint64_t EncodeMorton2(uint32_t x, uint32_t y) {
    return (LeftShift2(y) << 1) | LeftShift2(x);
}

inline uint32_t LeftShift3(uint32_t x) {
    // DCHECK_LE(x, (1u << 10));
    if (x == (1 << 10))
        --x;
    x = (x | (x << 16)) & 0b00000011000000000000000011111111;
    // x = ---- --98 ---- ---- ---- ---- 7654 3210
    x = (x | (x << 8)) & 0b00000011000000001111000000001111;
    // x = ---- --98 ---- ---- 7654 ---- ---- 3210
    x = (x | (x << 4)) & 0b00000011000011000011000011000011;
    // x = ---- --98 ---- 76-- --54 ---- 32-- --10
    x = (x | (x << 2)) & 0b00001001001001001001001001001001;
    // x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0
    return x;
}

inline uint32_t EncodeMorton3(float x, float y, float z) {
    // DCHECK_GE(x, 0);
    // DCHECK_GE(y, 0);
    // DCHECK_GE(z, 0);
    return (LeftShift3(z) << 2) | (LeftShift3(y) << 1) | LeftShift3(x);
}

inline uint32_t Compact1By1(uint64_t x) {
    // TODO: as of Haswell, the PEXT instruction could do all this in a
    // single instruction.
    // x = -f-e -d-c -b-a -9-8 -7-6 -5-4 -3-2 -1-0
    x &= 0x5555555555555555;
    // x = --fe --dc --ba --98 --76 --54 --32 --10
    x = (x ^ (x >> 1)) & 0x3333333333333333;
    // x = ---- fedc ---- ba98 ---- 7654 ---- 3210
    x = (x ^ (x >> 2)) & 0x0f0f0f0f0f0f0f0f;
    // x = ---- ---- fedc ba98 ---- ---- 7654 3210
    x = (x ^ (x >> 4)) & 0x00ff00ff00ff00ff;
    // x = ---- ---- ---- ---- fedc ba98 7654 3210
    x = (x ^ (x >> 8)) & 0x0000ffff0000ffff;
    // ...
    x = (x ^ (x >> 16)) & 0xffffffff;
    return x;
}

inline void DecodeMorton2(uint64_t v, uint32_t *x, uint32_t *y) {
    *x = Compact1By1(v);
    *y = Compact1By1(v >> 1);
}

inline uint32_t Compact1By2(uint32_t x) {
    x &= 0x09249249;                   // x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0
    x = (x ^ (x >> 2)) & 0x030c30c3;   // x = ---- --98 ---- 76-- --54 ---- 32-- --10
    x = (x ^ (x >> 4)) & 0x0300f00f;   // x = ---- --98 ---- ---- 7654 ---- ---- 3210
    x = (x ^ (x >> 8)) & 0xff0000ff;   // x = ---- --98 ---- ---- ---- ---- 7654 3210
    x = (x ^ (x >> 16)) & 0x000003ff;  // x = ---- ---- ---- ---- ---- --98 7654 3210
    return x;
}

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

inline float SmoothStep(float x, float a, float b) {
    if (a == b)
        return (x < a) ? 0 : 1;
    // DCHECK_LT(a, b);
    float t = Clamp((x - a) / (b - a), 0, 1);
    return t * t * (3 - 2 * t);
}

inline float SafeSqrt(float x) {
    // DCHECK_GE(x, -1e-3f);  // not too negative
    return std::sqrt(std::max(0.f, x));
}


inline double SafeSqrt(double x) {
    // DCHECK_GE(x, -1e-3);  // not too negative
    return std::sqrt(std::max(0., x));
}

template <typename T>
inline constexpr T Sqr(T v) {
    return v * v;
}

// Would be nice to allow Float to be a template type here, but it is tricky:
// https://stackoverflow.com/questions/5101516/why-function-template-cannot-be-partially-specialized
template <int n>
inline constexpr float Pow(float v) {
    if constexpr (n < 0)
        return 1 / Pow<-n>(v);
    float n2 = Pow<n / 2>(v);
    return n2 * n2 * Pow<n & 1>(v);
}

template <>
inline constexpr float Pow<1>(float v) {
    return v;
}
template <>
inline constexpr float Pow<0>(float v) {
    return 1;
}

template <int n>
inline constexpr double Pow(double v) {
    if constexpr (n < 0)
        return 1 / Pow<-n>(v);
    double n2 = Pow<n / 2>(v);
    return n2 * n2 * Pow<n & 1>(v);
}

template <>
inline constexpr double Pow<1>(double v) {
    return v;
}

template <>
 inline constexpr double Pow<0>(double v) {
    return 1;
}

inline float SafeASin(float x) {
    // DCHECK(x >= -1.0001 && x <= 1.0001);
    return std::asin(Clamp(x, -1, 1));
}
inline float SafeACos(float x) {
    // DCHECK(x >= -1.0001 && x <= 1.0001);
    return std::acos(Clamp(x, -1, 1));
}

// inline double SafeASin(double x) {
//     // DCHECK(x >= -1.0001 && x <= 1.0001);
//     return std::asin(Clamp(x, -1, 1));
// }

// inline double SafeACos(double x) {
//     // DCHECK(x >= -1.0001 && x <= 1.0001);
//     return std::acos(Clamp(x, -1, 1));
// }

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

// Permutation Inline Function Declarations
inline int PermutationElement(uint32_t i, uint32_t n, uint32_t seed);

inline int PermutationElement(uint32_t i, uint32_t l, uint32_t p) {
    uint32_t w = l - 1;
    w |= w >> 1;
    w |= w >> 2;
    w |= w >> 4;
    w |= w >> 8;
    w |= w >> 16;
    do {
        i ^= p;
        i *= 0xe170893d;
        i ^= p >> 16;
        i ^= (i & w) >> 4;
        i ^= p >> 8;
        i *= 0x0929eb3f;
        i ^= p >> 23;
        i ^= (i & w) >> 1;
        i *= 1 | p >> 27;
        i *= 0x6935fa69;
        i ^= (i & w) >> 11;
        i *= 0x74dcb303;
        i ^= (i & w) >> 2;
        i *= 0x9e501cc3;
        i ^= (i & w) >> 2;
        i *= 0xc860a3df;
        i &= w;
        i ^= i >> 5;
    } while (i >= l);
    return (i + p) % l;
}

} // namespace PBRender