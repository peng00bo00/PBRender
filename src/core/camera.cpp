#include "camera.h"

namespace PBRender {

// Camera Method Definitions
Camera::~Camera() {}

Camera::Camera(const Transform &CameraToWorld)
    : CameraToWorld(CameraToWorld) {
    if (CameraToWorld.HasScale())
        std::cerr <<
            "Scaling detected in world-to-camera transformation!\n"
            "The system has numerous assumptions, implicit and explicit,\n"
            "that this transform will have no scale factors in it.\n"
            "Proceed at your own risk; your image may have errors or\n"
            "the system may crash as a result of this."
        << std::endl;
}

// Camera::Camera(const Transform &CameraToWorld, float shutterOpen,
//                float shutterClose, Film *film)
//             //    const Medium *medium)
//     : CameraToWorld(CameraToWorld),
//       shutterOpen(shutterOpen),
//       shutterClose(shutterClose),
//       film(film)
//     //   medium(medium)
//     {
//     if (CameraToWorld.HasScale())
//         std::cout <<
//             "Scaling detected in world-to-camera transformation!\n"
//             "The system has numerous assumptions, implicit and explicit,\n"
//             "that this transform will have no scale factors in it.\n"
//             "Proceed at your own risk; your image may have errors or\n"
//             "the system may crash as a result of this." << std::endl;
// }

float Camera::GenerateRayDifferential(const CameraSample &sample,
                                      RayDifferential *rd) const {
    float wt = GenerateRay(sample, rd);
    if (wt == 0) return 0;

    // Find camera ray after shifting a fraction of a pixel in the $x$ direction
    float wtx;
    for (float eps : { .05, -.05 }) {
        CameraSample sshift = sample;
        sshift.pFilm.x += eps;
        Ray rx;
        wtx = GenerateRay(sshift, &rx);
        rd->rxOrigin = rd->o + (rx.o - rd->o) / eps;
        rd->rxDirection = rd->d + (rx.d - rd->d) / eps;
        if (wtx != 0)
            break;
    }
    if (wtx == 0)
        return 0;

    // Find camera ray after shifting a fraction of a pixel in the $y$ direction
    float wty;
    for (float eps : { .05, -.05 }) {
        CameraSample sshift = sample;
        sshift.pFilm.y += eps;
        Ray ry;
        wty = GenerateRay(sshift, &ry);
        rd->ryOrigin = rd->o + (ry.o - rd->o) / eps;
        rd->ryDirection = rd->d + (ry.d - rd->d) / eps;
        if (wty != 0)
            break;
    }
    if (wty == 0)
        return 0;

    rd->hasDifferentials = true;
    return wt;
}

// Spectrum Camera::We(const Ray &ray, Point2f *raster) const {
//     std::cerr << "Camera::We() is not implemented!";
//     return Spectrum(0.f);
// }

// void Camera::Pdf_We(const Ray &ray, float *pdfPos, float *pdfDir) const {
//     std::cerr << "Camera::Pdf_We() is not implemented!";
// }

// Spectrum Camera::Sample_Wi(const Interaction &ref, const Point2f &u,
//                            Vector3f *wi, float *pdf, Point2f *pRaster,
//                            VisibilityTester *vis) const {
//     LOG(FATAL) << "Camera::Sample_Wi() is not implemented!";
//     return Spectrum(0.f);
// }

}