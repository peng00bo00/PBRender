#pragma once

#include <embree4/rtcore.h>

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

inline void InitRTCRayHit(PBRender::Ray &ray, RTCRayHit &rayhit) {
    rayhit.ray.org_x = ray.org.x;
    rayhit.ray.org_y = ray.org.y;
    rayhit.ray.org_z = ray.org.z;
    rayhit.ray.dir_x = ray.dir.x;
    rayhit.ray.dir_y = ray.dir.y;
    rayhit.ray.dir_z = ray.dir.z;

    rayhit.ray.tnear = 0;
    rayhit.ray.tfar  = PBRender::Infinity;

    rayhit.ray.mask  =-1;
    rayhit.ray.flags = 0;
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
}

} // namespace PBRender