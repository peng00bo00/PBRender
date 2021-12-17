#pragma once

#include "PBRender.h"
#include "camera.h"

namespace PBRender {

// OrthographicCamera Declarations
class PerspectiveCamera : public ProjectiveCamera {
    public:
        // OrthographicCamera Public Methods
        // OrthographicCamera(const AnimatedTransform &CameraToWorld,
        //                 const Bounds2f &screenWindow, float shutterOpen,
        //                 float shutterClose, float lensRadius,
        //                 float focalDistance, Film *film, const Medium *medium)
        //     : ProjectiveCamera(CameraToWorld, Orthographic(0, 1), screenWindow,
        //                     shutterOpen, shutterClose, lensRadius, focalDistance,
        //                     film, medium) {
        //     // Compute differential changes in origin for orthographic camera rays
        //     dxCamera = RasterToCamera(Vector3f(1, 0, 0));
        //     dyCamera = RasterToCamera(Vector3f(0, 1, 0));
        // }

        PerspectiveCamera(const Transform &CameraToWorld,
                          const Bounds2f &screenWindow,
                          const Vector2f &fullResolution,
                          float lensRadius,
                          float focalDistance,
                          float fov);
    

        float GenerateRay(const CameraSample &sample, Ray *) const;
        // float GenerateRayDifferential(const CameraSample &sample,
        //                               RayDifferential *) const;

    private:
        // OrthographicCamera Private Data
        Vector3f dxCamera, dyCamera;
        float A;
};

PerspectiveCamera *CreatePerspectiveCamera(const Transform &cam2world, const Vector2f &fullResolution,
                                           const float fov=90.0f, const float lensradius=0.0f, const float focaldistance=0.0f); 

}