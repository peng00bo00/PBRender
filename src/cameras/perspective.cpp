#include <PBRender/cameras/perspective.h>

namespace PBRender
{

PerspectiveCamera::PerspectiveCamera(const Transform &CameraToWorld,
                                    const Bounds2f &screenWindow,
                                    const Vector2f &fullResolution,
                                    float lensRadius,
                                    float focalDistance,
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

std::shared_ptr<PerspectiveCamera> CreatePerspectiveCamera(const Transform &cam2world, const Vector2f &fullResolution,
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

    return std::make_shared<PerspectiveCamera>(cam2world, screen, fullResolution, lensradius, focaldistance, fov);
}

} // namespace PBRender