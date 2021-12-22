#pragma once

#include "PBRender.h"

#include "geometry.h"
#include "transform.h"

namespace PBRender {

// Camera Declarations
class Camera {
    public:
        // Camera(const AnimatedTransform &CameraToWorld, Float shutterOpen,
        //    Float shutterClose, Film *film, const Medium *medium);

        Camera(const Transform &CameraToWorld);
        virtual ~Camera();

        virtual float GenerateRay(const CameraSample &sample, Ray *ray) const = 0;
        // virtual float GenerateRayDifferential(const CameraSample &sample, RayDifferential *rd) const;

        // virtual Spectrum We(const Ray &ray, Point2f *pRaster2 = nullptr) const;
        // virtual void Pdf_We(const Ray &ray, float *pdfPos, float *pdfDir) const;

        // virtual Spectrum Sample_Wi(const Interaction &ref, const Point2f &u,
        //                            Vector3f *wi, float *pdf, Point2f *pRaster,
        //                            VisibilityTester *vis) const;

    public:
        // Camera Public Data
        Transform CameraToWorld;
        // const float shutterOpen, shutterClose;
        // Film *film;
        // const Medium *medium;
};

struct CameraSample {
    Point2f pFilm;
    Point2f pLens;
    float time;
};

inline std::ostream &operator<<(std::ostream &os, const CameraSample &cs) {
    os << "[ pFilm: " << cs.pFilm << " , pLens: " << cs.pLens <<
          ", time: " << cs.time << " ]";
    return os;
}

class ProjectiveCamera : public Camera {
    public:
        // ProjectiveCamera Public Methods
        // ProjectiveCamera(const AnimatedTransform &CameraToWorld,
        //                 const Transform &CameraToScreen,
        //                 const Bounds2f &screenWindow, Float shutterOpen,
        //                 Float shutterClose, Float lensr, Float focald, Film *film,
        //                 const Medium *medium)
        //     : Camera(CameraToWorld, shutterOpen, shutterClose, film, medium),
        //     CameraToScreen(CameraToScreen) {
        //     // Initialize depth of field parameters
        //     lensRadius = lensr;
        //     focalDistance = focald;

        //     // Compute projective camera transformations

        //     // Compute projective camera screen transformations
        //     ScreenToRaster =
        //         Scale(film->fullResolution.x, film->fullResolution.y, 1) *
        //         Scale(1 / (screenWindow.pMax.x - screenWindow.pMin.x),
        //             1 / (screenWindow.pMin.y - screenWindow.pMax.y), 1) *
        //         Translate(Vector3f(-screenWindow.pMin.x, -screenWindow.pMax.y, 0));
        //     RasterToScreen = Inverse(ScreenToRaster);
        //     RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;
        // }
        ProjectiveCamera (const Transform &CameraToWorld,
                          const Transform &CameraToScreen,
                          const Bounds2f &screenWindow,
                          const Vector2f &fullResolution,
                          float lensr, float focald
                          )
            : Camera(CameraToWorld), CameraToScreen(CameraToScreen) {
            // Initialize depth of field parameters
            lensRadius = lensr;
            focalDistance = focald;

            // Compute projective camera transformations

            // Compute projective camera screen transformations
            ScreenToRaster =
                             Scale(fullResolution.x, fullResolution.y, 1) *
                             Scale(1 / (screenWindow.pMax.x - screenWindow.pMin.x),
                                   1 / (screenWindow.pMin.y - screenWindow.pMax.y), 1) *
                             Translate(Vector3f(-screenWindow.pMin.x, -screenWindow.pMax.y, 0));
            
            RasterToScreen = Inverse(ScreenToRaster);
            RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;
        }
    
    protected:
        // ProjectiveCamera Protected Data
        Transform CameraToScreen, RasterToCamera;
        Transform ScreenToRaster, RasterToScreen;
        float lensRadius, focalDistance;

};

}