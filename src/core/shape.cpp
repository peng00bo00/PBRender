#include "shape.h"

namespace PBRender {

long long Shape::nShapesCreated = 0;

Shape::Shape(const Transform *ObjectToWorld, const Transform *WorldToObject,
             bool reverseOrientation)
    : ObjectToWorld(ObjectToWorld),
      WorldToObject(WorldToObject),
      reverseOrientation(reverseOrientation) {
    ++nShapesCreated;
}

Shape::~Shape() {}

Bounds3f Shape::WorldBound() const { return (*ObjectToWorld)(ObjectBound()); }

}