#include "cameras/orthographic.h"
#include "sampling.h"

namespace PBRender {

// OrthographicCamera Definitions
float OrthographicCamera::GenerateRay(const CameraSample &sample,
                                      Ray *ray) const {
    // ProfilePhase prof(Prof::GenerateCameraRay);
    // Compute raster and camera sample positions
    Point3f pFilm = Point3f(sample.pFilm.x, sample.pFilm.y, 0);
    Point3f pCamera = RasterToCamera(pFilm);
    *ray = Ray(pCamera, Vector3f(0, 0, 1));

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

float OrthographicCamera::GenerateRayDifferential(const CameraSample &sample,
                                                  RayDifferential *ray) const {
    // ProfilePhase prof(Prof::GenerateCameraRay);
    // Compute main orthographic viewing ray

    // Compute raster and camera sample positions
    Point3f pFilm = Point3f(sample.pFilm.x, sample.pFilm.y, 0);
    Point3f pCamera = RasterToCamera(pFilm);
    *ray = RayDifferential(pCamera, Vector3f(0, 0, 1));

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

    // Compute ray differentials for _OrthographicCamera_
    if (lensRadius > 0) {
        // Compute _OrthographicCamera_ ray differentials accounting for lens

        // Sample point on lens
        Point2f pLens = lensRadius * ConcentricSampleDisk(sample.pLens);
        float ft = focalDistance / ray->d.z;

        Point3f pFocus = pCamera + dxCamera + (ft * Vector3f(0, 0, 1));
        ray->rxOrigin = Point3f(pLens.x, pLens.y, 0);
        ray->rxDirection = Normalize(pFocus - ray->rxOrigin);

        pFocus = pCamera + dyCamera + (ft * Vector3f(0, 0, 1));
        ray->ryOrigin = Point3f(pLens.x, pLens.y, 0);
        ray->ryDirection = Normalize(pFocus - ray->ryOrigin);
    } else {
        ray->rxOrigin = ray->o + dxCamera;
        ray->ryOrigin = ray->o + dyCamera;
        ray->rxDirection = ray->ryDirection = ray->d;
    }
    // ray->time = Lerp(sample.time, shutterOpen, shutterClose);
    ray->hasDifferentials = true;
    // ray->medium = medium;
    *ray = CameraToWorld(*ray);
    return 1;
}

OrthographicCamera *CreateOrthographicCamera(const Transform &cam2world, const Vector2f &fullResolution, const float lensradius, const float focaldistance) {
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

    float ScreenScale = 1.0f;

    {
        screen.pMin.x *= ScreenScale;
        screen.pMax.x *= ScreenScale;
        screen.pMin.y *= ScreenScale;
        screen.pMax.y *= ScreenScale;
    }

    return new OrthographicCamera(cam2world, screen, fullResolution, lensradius, focaldistance);
}

// OrthographicCamera *CreateOrthographicCamera(const Transform &cam2world, const Vector2f &fullResolution) {
//     return CreateOrthographicCamera(cam2world, fullResolution, 0.0f, 0.0f);
// }

}