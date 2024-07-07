#pragma once

#include <PBRender/core/common.h>
#include <PBRender/core/math.h>
#include <PBRender/core/rng.h>
#include <PBRender/core/vecmath.h>

namespace PBRender
{

inline Point2f SampleUniformDiskPolar(Point2f u) {
    float r = std::sqrt(u[0]);
    float theta = 2 * Pi * u[1];
    return {r * std::cos(theta), r * std::sin(theta)};
}

inline Point2f SampleUniformDiskConcentric(Point2f u) {
    // Map _u_ to $[-1,1]^2$ and handle degeneracy at the origin
    Point2f uOffset = 2 * u - Vector2f(1, 1);
    if (uOffset.x == 0 && uOffset.y == 0)
        return {0, 0};

    // Apply concentric mapping to point
    float theta, r;
    if (std::abs(uOffset.x) > std::abs(uOffset.y)) {
        r = uOffset.x;
        theta = PiOver4 * (uOffset.y / uOffset.x);
    } else {
        r = uOffset.y;
        theta = PiOver2 - PiOver4 * (uOffset.x / uOffset.y);
    }
    return r * Point2f(std::cos(theta), std::sin(theta));
}

inline Vector3f SampleUniformHemisphere(Point2f u) {
    float z = u[0];
    float r = SafeSqrt(1 - Sqr(z));
    float phi = 2 * Pi * u[1];
    return {r * std::cos(phi), r * std::sin(phi), z};
}

inline float UniformHemispherePDF() {
    return Inv2Pi;
}

inline Point2f InvertUniformHemisphereSample(Vector3f w) {
    float phi = std::atan2(w.y, w.x);
    if (phi < 0)
        phi += 2 * Pi;
    return Point2f(w.z, phi / (2 * Pi));
}

inline Vector3f SampleUniformSphere(Point2f u) {
    float z = 1 - 2 * u[0];
    float r = SafeSqrt(1 - Sqr(z));
    float phi = 2 * Pi * u[1];
    return {r * std::cos(phi), r * std::sin(phi), z};
}

inline float UniformSpherePDF() {
    return Inv4Pi;
}

inline Point2f InvertUniformSphereSample(Vector3f w) {
    float phi = std::atan2(w.y, w.x);
    if (phi < 0)
        phi += 2 * Pi;
    return Point2f((1 - w.z) / 2, phi / (2 * Pi));
}

inline Vector3f SampleCosineHemisphere(Point2f u) {
    Point2f d = SampleUniformDiskConcentric(u);
    float z = SafeSqrt(1 - Sqr(d.x) - Sqr(d.y));
    return Vector3f(d.x, d.y, z);
}

inline float CosineHemispherePDF(float cosTheta) {
    return cosTheta * InvPi;
}

} // namespace PBRender