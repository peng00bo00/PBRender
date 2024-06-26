#pragma once

#include <PBRender/core/common.h>
#include <PBRender/core/vecmath.h>


namespace PBRender
{

class Ray {
public:
    Ray() : tfar(Infinity), tnear(0.f) {}
    Ray(const Point3f &o, const Vector3f &d, float tnear = 0.f,
        float tfar = Infinity)
            : org(o), dir(d), tnear(tnear), tfar(tfar) {}

    Point3f operator()(float t) const { return org + dir * t; }

public:
    Point3f org;
    Vector3f dir;
    float tnear;
    float tfar;
};

inline std::ostream &operator<<(std::ostream &os, const Ray &ray) {
    os << "Ray Origin: " << ray.org << ", " << "Ray Dir: " << ray.dir;
    return os;
}

} // namespace PBRender