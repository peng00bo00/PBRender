#pragma once

#include <PBRender/core/common.h>
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
    Camera(const Transform &CameraToWorld, Film *film);
    virtual ~Camera();

    virtual float GenerateRay(const CameraSample &sample, Ray &ray) const = 0;
    // virtual float GenerateRayDifferential(const CameraSample &sample, RayDifferential *rd) const;
    // float GenerateRay(const CameraSample &sample, Ray *ray);

    Film* GetFilm() const { return film; }

protected:
    Transform CameraToWorld;

    // TODO: shutterOpen, shutterClose used in motion blur, skipped for now
    // float shutterOpen, shutterClose;
    Film *film;

    // TODO: medium used in volume rendering, skipped for now
    // const Medium *medium;
};


class ProjectiveCamera : public Camera {
public:
    ProjectiveCamera (const Transform &CameraToWorld,
                    const Transform &CameraToScreen,
                    const Bounds2f &screenWindow,
                    Film *film,
                    float lensr, float focald
                    )
    : Camera(CameraToWorld, film), CameraToScreen(CameraToScreen) {
    // Initialize depth of field parameters
    lensRadius = lensr;
    focalDistance = focald;
    Point2i fullResolution = film->FullResolution();

    ScreenToRaster = Scale(fullResolution.x, fullResolution.y, 1) *
                     Scale(1 / (screenWindow.pMax.x - screenWindow.pMin.x),
                        1 / (screenWindow.pMin.y - screenWindow.pMax.y), 1) *
                    Translate(Vector3f(-screenWindow.pMin.x, -screenWindow.pMax.y, 0));
            
        RasterToScreen = ScreenToRaster.Inverse();
        RasterToCamera = CameraToScreen.Inverse() * RasterToScreen;
    }

protected:
    Transform CameraToScreen, RasterToCamera;
    Transform ScreenToRaster, RasterToScreen;
    float lensRadius, focalDistance;
};

} // namespace PBRender