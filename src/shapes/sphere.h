#include "shape.h"

namespace PBRender {

class Sphere : public Shape {
    public:
        Sphere (const Transform *ObjectToWorld, const Transform *WorldToObject,
                bool reverseOrientation, float radius)
                : Shape(ObjectToWorld, WorldToObject, reverseOrientation),
                  radius(radius)
                {}

        Bounds3f ObjectBound() const;

        bool Intersect(const Ray &ray, float *tHit, SurfaceInteraction *isect,
                       bool testAlphaTexture) const;
        bool IntersectP(const Ray &ray, bool testAlphaTexture) const;
        
        float Area() const;

    private:
        const float radius;
};

}