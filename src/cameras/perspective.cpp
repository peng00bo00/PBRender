#include "cameras/perspective.h"
#include "sampling.h"

namespace PBRender {

// PerspectiveCamera Method Definitions
PerspectiveCamera::PerspectiveCamera(const Transform &CameraToWorld,
                                     const Bounds2f &screenWindow,
                                     const Vector2f &fullResolution,
                                     float lensRadius, float focalDistance,
                                     float fov)
    : ProjectiveCamera(CameraToWorld, Perspective(fov, 1e-2f, 1000.f),
                       screenWindow, fullResolution, lensRadius, focalDistance) {
    // Compute differential changes in origin for perspective camera rays
    dxCamera =
        (RasterToCamera(Point3f(1, 0, 0)) - RasterToCamera(Point3f(0, 0, 0)));
    dyCamera =
        (RasterToCamera(Point3f(0, 1, 0)) - RasterToCamera(Point3f(0, 0, 0)));

    // Compute image plane bounds at $z=1$ for _PerspectiveCamera_
    Point2i res = Point2i(fullResolution);
    Point3f pMin = RasterToCamera(Point3f(0, 0, 0));
    Point3f pMax = RasterToCamera(Point3f(res.x, res.y, 0));
    pMin /= pMin.z;
    pMax /= pMax.z;
    A = std::abs((pMax.x - pMin.x) * (pMax.y - pMin.y));
}

float PerspectiveCamera::GenerateRay(const CameraSample &sample,
                                     Ray *ray) const {
    // ProfilePhase prof(Prof::GenerateCameraRay);
    // Compute raster and camera sample positions
    Point3f pFilm = Point3f(sample.pFilm.x, sample.pFilm.y, 0);
    Point3f pCamera = RasterToCamera(pFilm);
    *ray = Ray(Point3f(0, 0, 0), Normalize(Vector3f(pCamera)));

    // Modify ray for depth of field
    if (lensRadius > 0) {
        // Sample point on lens
        Point2f pLens = lensRadius * ConcentricSampleDisk(sample.pLens);

        // Compute point on plane of focus
        float ft = focalDistance / ray->d.z;
        Point3f pFocus = (*ray)(ft);

        // Update ray for effect of lens
        ray->o = Point3f(pLens.x, pLens.y, 0);
        ray->d = Normalize(pFocus - ray->o);
    }

    // ray->time = Lerp(sample.time, shutterOpen, shutterClose);
    // ray->medium = medium;
    *ray = CameraToWorld(*ray);
    return 1;
}

float PerspectiveCamera::GenerateRayDifferential(const CameraSample &sample,
                                                 RayDifferential *ray) const {
    // ProfilePhase prof(Prof::GenerateCameraRay);
    // Compute raster and camera sample positions
    Point3f pFilm = Point3f(sample.pFilm.x, sample.pFilm.y, 0);
    Point3f pCamera = RasterToCamera(pFilm);
    Vector3f dir = Normalize(Vector3f(pCamera.x, pCamera.y, pCamera.z));
    *ray = RayDifferential(Point3f(0, 0, 0), dir);

    // Modify ray for depth of field
    if (lensRadius > 0) {
        // Sample point on lens
        Point2f pLens = lensRadius * ConcentricSampleDisk(sample.pLens);

        // Compute point on plane of focus
        float ft = focalDistance / ray->d.z;
        Point3f pFocus = (*ray)(ft);

        // Update ray for effect of lens
        ray->o = Point3f(pLens.x, pLens.y, 0);
        ray->d = Normalize(pFocus - ray->o);
    }

    // Compute offset rays for _PerspectiveCamera_ ray differentials
    if (lensRadius > 0) {
        // Compute _PerspectiveCamera_ ray differentials accounting for lens

        // Sample point on lens
        Point2f pLens = lensRadius * ConcentricSampleDisk(sample.pLens);
        Vector3f dx = Normalize(Vector3f(pCamera + dxCamera));
        float ft = focalDistance / dx.z;
        Point3f pFocus = Point3f(0, 0, 0) + (ft * dx);
        ray->rxOrigin = Point3f(pLens.x, pLens.y, 0);
        ray->rxDirection = Normalize(pFocus - ray->rxOrigin);

        Vector3f dy = Normalize(Vector3f(pCamera + dyCamera));
        ft = focalDistance / dy.z;
        pFocus = Point3f(0, 0, 0) + (ft * dy);
        ray->ryOrigin = Point3f(pLens.x, pLens.y, 0);
        ray->ryDirection = Normalize(pFocus - ray->ryOrigin);
    } else {
        ray->rxOrigin = ray->ryOrigin = ray->o;
        ray->rxDirection = Normalize(Vector3f(pCamera) + dxCamera);
        ray->ryDirection = Normalize(Vector3f(pCamera) + dyCamera);
    }

    // ray->time = Lerp(sample.time, shutterOpen, shutterClose);
    // ray->medium = medium;
    *ray = CameraToWorld(*ray);
    ray->hasDifferentials = true;

    return 1;
}

PerspectiveCamera *CreatePerspectiveCamera(const Transform &cam2world, const Vector2f &fullResolution,
                                           const float fov, const float lensradius, const float focaldistance) {
    float frame = fullResolution.x / fullResolution.y;
    Bounds2f screen;

    if (frame > 1.f) {
        screen.pMin.x = -frame;
        screen.pMax.x = frame;
        screen.pMin.y = -1.f;
        screen.pMax.y = 1.f;
    } else {
        screen.pMin.x = -1.f;
        screen.pMax.x = 1.f;
        screen.pMin.y = -1.f / frame;
        screen.pMax.y = 1.f / frame;
    }

    return new PerspectiveCamera(cam2world, screen, fullResolution, lensradius, focaldistance, fov);
}

}