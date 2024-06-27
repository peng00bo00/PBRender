#include <PBRender/cameras/perspective.h>

namespace PBRender
{

PerspectiveCamera::PerspectiveCamera(const Transform &CameraToWorld,
                                    const Bounds2f &screenWindow,
                                    Film &film,
                                    float lensRadius,
                                    float focalDistance,
                                    float fov)
    : ProjectiveCamera(CameraToWorld, Perspective(fov, 1e-2f, 1000.f),
                       screenWindow, film, lensRadius, focalDistance) {
    // Compute differential changes in origin for perspective camera rays
    dxCamera =
        (RasterToCamera(Point3f(1, 0, 0)) - RasterToCamera(Point3f(0, 0, 0)));
    dyCamera =
        (RasterToCamera(Point3f(0, 1, 0)) - RasterToCamera(Point3f(0, 0, 0)));

    // Compute image plane bounds at $z=1$ for _PerspectiveCamera_
    Point2i res = film.FullResolution();
    Point3f pMin = RasterToCamera(Point3f(0, 0, 0));
    Point3f pMax = RasterToCamera(Point3f(res.x, res.y, 0));
    pMin /= pMin.z;
    pMax /= pMax.z;
    A = std::abs((pMax.x - pMin.x) * (pMax.y - pMin.y));
}

float PerspectiveCamera::GenerateRay(const CameraSample &sample,
                                     Ray &ray) const {
    // Compute raster and camera sample positions
    Point3f pFilm = Point3f(sample.pFilm.x, sample.pFilm.y, 0);
    Point3f pCamera = RasterToCamera(pFilm);
    ray = Ray(Point3f(0, 0, 0), Normalize(Vector3f(pCamera)));

    // Modify ray for depth of field
    // if (lensRadius > 0) {
    //     // Sample point on lens
    //     Point2f pLens = lensRadius * ConcentricSampleDisk(sample.pLens);

    //     // Compute point on plane of focus
    //     float ft = focalDistance / ray->d.z;
    //     Point3f pFocus = (*ray)(ft);

    //     // Update ray for effect of lens
    //     ray->o = Point3f(pLens.x, pLens.y, 0);
    //     ray->d = Normalize(pFocus - ray->o);
    // }

    // ray->time = Lerp(sample.time, shutterOpen, shutterClose);
    // ray->medium = medium;
    ray = CameraToWorld(ray);
    return 1;
}

std::shared_ptr<PerspectiveCamera> CreatePerspectiveCamera(const Transform &cam2world, Film &film,
                                           const float fov, const float lensradius, const float focaldistance) {
    
    Point2i fullResolution = film.FullResolution();
    float frame = static_cast<float>(fullResolution.x) / static_cast<float>(fullResolution.y);
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

    return std::make_shared<PerspectiveCamera>(cam2world, screen, film, lensradius, focaldistance, fov);
}

} // namespace PBRender