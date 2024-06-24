#pragma once

#include <PBRender/core/common.h>
#include <PBRender/core/math.h>
#include <PBRender/core/vecmath.h>
#include <PBRender/core/transform.h>
#include <PBRender/core/film.h>

namespace PBRender
{

struct CameraSample {
    Point2f pFilm;
    Point2f pLens;
    float time         = 0;
    float filterWeight = 1;
};

class Camera {
public:
    Camera(const Transform &CameraToWorld);
    virtual ~Camera();

    virtual float GenerateRay(const CameraSample &sample, Ray *ray) const = 0;
    // virtual float GenerateRayDifferential(const CameraSample &sample, RayDifferential *rd) const;

    Film GetFilm() const { return film; }

protected:
    Transform CameraToWorld;
    float shutterOpen, shutterClose;
    Film film;
};


class ProjectiveCamera : Camera {

};

} // namespace PBRender