#pragma once

#include <PBRender/core/camera.h>

namespace PBRender
{

class OrthographicCamera : public ProjectiveCamera {
    public:
        OrthographicCamera(const Transform &CameraToWorld,
                           const Bounds2f &screenWindow,
                           const Vector2f &fullResolution,
                           float lensRadius,
                           float focalDistance)
            : ProjectiveCamera(CameraToWorld, Orthographic(0, 1), screenWindow,
                               fullResolution, lensRadius, focalDistance) {
            dxCamera = RasterToCamera(Vector3f(1, 0, 0));
            dyCamera = RasterToCamera(Vector3f(0, 1, 0));
        }
    

        float GenerateRay(const CameraSample &sample, Ray &) const;
        // float GenerateRayDifferential(const CameraSample &sample,
        //                               RayDifferential *) const;

    private:
        // OrthographicCamera Private Data
        Vector3f dxCamera, dyCamera;
};

std::shared_ptr<OrthographicCamera> CreateOrthographicCamera(const Transform &cam2world, const Vector2f &fullResolution, const float lensradius=0.0f, const float focaldistance=0.0f);

} // namespace PBRender