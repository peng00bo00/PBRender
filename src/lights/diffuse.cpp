#include "lights/diffuse.h"
// #include "paramset.h"
#include "sampling.h"
#include "shapes/triangle.h"
// #include "stats.h"

namespace PBRender {

DiffuseAreaLight::DiffuseAreaLight(const Transform &LightToWorld,
                                //    const MediumInterface &mediumInterface,
                                   const Spectrum &Lemit, int nSamples,
                                   const std::shared_ptr<Shape> &shape,
                                   bool twoSided)
    : AreaLight(LightToWorld, nSamples),
      Lemit(Lemit),
      shape(shape),
      twoSided(twoSided),
      area(shape->Area()) {
    if (WorldToLight.HasScale() &&
        dynamic_cast<const Triangle *>(shape.get()) == nullptr)
        std::cerr << "Scaling detected in world to light transformation! "
                     "The system has numerous assumptions, implicit and explicit, "
                     "that this transform will have no scale factors in it. "
                     "Proceed at your own risk; your image may have errors."
                  << std::endl;
}

Spectrum DiffuseAreaLight::Power() const {
    return (twoSided ? 2 : 1) * Lemit * area * Pi;
}

Spectrum DiffuseAreaLight::Sample_Li(const Interaction &ref, const Point2f &u,
                                     Vector3f *wi, float *pdf,
                                     VisibilityTester *vis) const {
    // ProfilePhase _(Prof::LightSample);
    Interaction pShape = shape->Sample(ref, u, pdf);
    // pShape.mediumInterface = mediumInterface;
    if (*pdf == 0 || (pShape.p - ref.p).LengthSquared() == 0) {
        *pdf = 0;
        return 0.f;
    }
    *wi = Normalize(pShape.p - ref.p);
    *vis = VisibilityTester(ref, pShape);
    return L(pShape, -*wi);
}

float DiffuseAreaLight::Pdf_Li(const Interaction &ref,
                               const Vector3f &wi) const {
    // ProfilePhase _(Prof::LightPdf);
    return shape->Pdf(ref, wi);
}

Spectrum DiffuseAreaLight::Sample_Le(const Point2f &u1, const Point2f &u2,
                                     float time, Ray *ray, Normal3f *nLight,
                                     float *pdfPos, float *pdfDir) const {
    // ProfilePhase _(Prof::LightSample);

    // Sample a point on the area light's _Shape_, _pShape_
    Interaction pShape = shape->Sample(u1, pdfPos);
    // pShape.mediumInterface = mediumInterface;
    *nLight = pShape.n;

    // Sample a cosine-weighted outgoing direction _w_ for area light
    Vector3f w;
    if (twoSided) {
        Point2f u = u2;
        // Choose a side to sample and then remap u[0] to [0,1] before
        // applying cosine-weighted hemisphere sampling for the chosen side.
        if (u[0] < .5) {
            u[0] = std::min(u[0] * 2, OneMinusEpsilon);
            w = CosineSampleHemisphere(u);
        } else {
            u[0] = std::min((u[0] - .5f) * 2, OneMinusEpsilon);
            w = CosineSampleHemisphere(u);
            w.z *= -1;
        }
        *pdfDir = 0.5f * CosineHemispherePdf(std::abs(w.z));
    } else {
        w = CosineSampleHemisphere(u2);
        *pdfDir = CosineHemispherePdf(w.z);
    }

    Vector3f v1, v2, n(pShape.n);
    CoordinateSystem(n, &v1, &v2);
    w = w.x * v1 + w.y * v2 + w.z * n;
    *ray = pShape.SpawnRay(w);
    return L(pShape, w);
}

void DiffuseAreaLight::Pdf_Le(const Ray &ray, const Normal3f &n, float *pdfPos,
                              float *pdfDir) const {
    // ProfilePhase _(Prof::LightPdf);
    Interaction it(ray.o, n, Vector3f(), Vector3f(n), ray.time);
                //    mediumInterface);
    *pdfPos = shape->Pdf(it);
    *pdfDir = twoSided ? (.5 * CosineHemispherePdf(AbsDot(n, ray.d)))
                       : CosineHemispherePdf(Dot(n, ray.d));
}

}