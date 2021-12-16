#pragma once

#include "PBRender.h"
#include "camera.h"

namespace PBRender {

// PerspectiveCamera Declarations
class PerspectiveCamera : public ProjectiveCamera {
    public:
        // PerspectiveCamera Public Methods
        // PerspectiveCamera(const AnimatedTransform &CameraToWorld,
        //                   const Bounds2f &screenWindow, Float shutterOpen,
        //                   Float shutterClose, Float lensRadius, Float focalDistance,
        //                   Float fov, Film *film, const Medium *medium);

        PerspectiveCamera(const Transform &CameraToWorld,
                                            const Bounds2f &screenWindow,
                                            const Vector2f &fullResolution,
                                            float lensRadius, float focalDistance,
                                            float fov)
            : ProjectiveCamera(CameraToWorld, Perspective(fov, 1e-2f, 1000.f),
                            screenWindow, fullResolution,
                            lensRadius,
                            focalDistance) {
            // Compute differential changes in origin for perspective camera rays
            dxCamera =
                (RasterToCamera(Point3f(1, 0, 0)) - RasterToCamera(Point3f(0, 0, 0)));
            dyCamera =
                (RasterToCamera(Point3f(0, 1, 0)) - RasterToCamera(Point3f(0, 0, 0)));

            // Compute image plane bounds at $z=1$ for _PerspectiveCamera_
            Point2i res(fullResolution.x, fullResolution.y);
            Point3f pMin = RasterToCamera(Point3f(0, 0, 0));
            Point3f pMax = RasterToCamera(Point3f(res.x, res.y, 0));
            pMin /= pMin.z;
            pMax /= pMax.z;
            A = std::abs((pMax.x - pMin.x) * (pMax.y - pMin.y));
        }

        
        float GenerateRay(const CameraSample &sample, Ray *) const;
        // Float GenerateRayDifferential(const CameraSample &sample,
        //                               RayDifferential *ray) const;
        // Spectrum We(const Ray &ray, Point2f *pRaster2 = nullptr) const;
        // void Pdf_We(const Ray &ray, Float *pdfPos, Float *pdfDir) const;
        // Spectrum Sample_Wi(const Interaction &ref, const Point2f &sample,
        //                    Vector3f *wi, Float *pdf, Point2f *pRaster,
        //                    VisibilityTester *vis) const;

    private:
        // PerspectiveCamera Private Data
        Vector3f dxCamera, dyCamera;
        float A;
};

// PerspectiveCamera *CreatePerspectiveCamera(const ParamSet &params,
//                                            const AnimatedTransform &cam2world,
//                                            Film *film, const Medium *medium);
PerspectiveCamera  *CreatePerspectiveCamera(const Transform &cam2world, const Vector2f &fullResolution);

}