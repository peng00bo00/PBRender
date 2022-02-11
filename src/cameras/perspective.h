#pragma once

#include "PBRender.h"
#include "camera.h"

namespace PBRender {

// OrthographicCamera Declarations
class PerspectiveCamera : public ProjectiveCamera {
    public:
        PerspectiveCamera(const Transform &CameraToWorld,
                          const Bounds2f &screenWindow,
                          const Vector2f &fullResolution,
                          float lensRadius,
                          float focalDistance,
                          float fov);

        float GenerateRay(const CameraSample &sample, Ray *) const;
        float GenerateRayDifferential(const CameraSample &sample, 
                                      RayDifferential *ray) const;

    private:
        // OrthographicCamera Private Data
        Vector3f dxCamera, dyCamera;
        float A;
};

PerspectiveCamera *CreatePerspectiveCamera(const Transform &cam2world, const Vector2f &fullResolution,
                                           const float fov=90.0f, const float lensradius=0.0f, const float focaldistance=0.0f); 

}