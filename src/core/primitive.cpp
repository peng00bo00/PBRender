#include "primitive.h"

namespace PBRender {

static long long primitiveMemory = 0;

Primitive::~Primitive() {}

GeometricPrimitive::GeometricPrimitive(const std::shared_ptr<Shape> &shape)
    : shape(shape) {
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

void GeometricPrimitive::ComputeScatteringFunctions() const {

}

void Aggregate::ComputeScatteringFunctions() const {

}

}