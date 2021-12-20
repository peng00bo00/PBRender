#include "primitive.h"
#include "interaction.h"

namespace PBRender {

static long long primitiveMemory = 0;

Primitive::~Primitive() {}

const Material *Aggregate::GetMaterial() const {
    std::cerr <<
        "Aggregate::GetMaterial() method"
        "called; should have gone to GeometricPrimitive"
        << std::endl;
    return nullptr;
}

void Aggregate::ComputeScatteringFunctions(SurfaceInteraction *isect,
                                           TransportMode mode,
                                           bool allowMultipleLobes) const {
    std::cerr <<
        "Aggregate::ComputeScatteringFunctions() method"
        "called; should have gone to GeometricPrimitive"
        << std::endl;
}

GeometricPrimitive::GeometricPrimitive(const std::shared_ptr<Shape> &shape,
                                       const std::shared_ptr<Material> &material    )
    : shape(shape),
      material(material) {
    primitiveMemory += sizeof(*this);
}

Bounds3f GeometricPrimitive::WorldBound() const { return shape->WorldBound(); }

bool GeometricPrimitive::IntersectP(const Ray &r) const {
    return shape->IntersectP(r);
}

bool GeometricPrimitive::Intersect(const Ray &r,
                                   SurfaceInteraction *isect) const {
    float tHit;
    if (!shape->Intersect(r, &tHit, isect)) return false;
    r.tMax = tHit;
    isect->primitive = this;
    assert(Dot(isect->n, isect->shading.n) > 0.);

    // Initialize _SurfaceInteraction::mediumInterface_ after _Shape_ intersection
    // if (mediumInterface.IsMediumTransition())
    //     isect->mediumInterface = mediumInterface;
    // else
    //     isect->mediumInterface = MediumInterface(r.medium);

    return true;
}

void GeometricPrimitive::ComputeScatteringFunctions(
    SurfaceInteraction *isect,
    TransportMode mode,
    bool allowMultipleLobes) const {
    // ProfilePhase p(Prof::ComputeScatteringFuncs);
    if (material)
        material->ComputeScatteringFunctions(isect, mode,
                                             allowMultipleLobes);
    // CHECK_GE(Dot(isect->n, isect->shading.n), 0.);
    assert(Dot(isect->n, isect->shading.n) > 0.);
}

}