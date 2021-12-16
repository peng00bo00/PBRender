#pragma once

#include "PBRender.h"
#include "camera.h"

namespace PBRender {

// OrthographicCamera Declarations
class OrthographicCamera : public ProjectiveCamera {
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
    

        float GenerateRay(const CameraSample &sample, Ray *) const;
        // float GenerateRayDifferential(const CameraSample &sample,
        //                               RayDifferential *) const;

    private:
        // OrthographicCamera Private Data
        Vector3f dxCamera, dyCamera;
};

// OrthographicCamera *CreateOrthographicCamera(const ParamSet &params,
//                                              const AnimatedTransform &cam2world,
//                                              Film *film, const Medium *medium);

OrthographicCamera *CreateOrthographicCamera(const Transform &cam2world, const Vector2f &fullResolution);

}
