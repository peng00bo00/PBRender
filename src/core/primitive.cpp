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
    return true;
}

void GeometricPrimitive::ComputeScatteringFunctions() const {

}

}