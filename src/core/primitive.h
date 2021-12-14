#pragma once

#include "PBRender.h"
#include "shape.h"
#include "transform.h"

namespace PBRender {


class Primitive {
    public:
        virtual ~Primitive();
        virtual Bounds3f WorldBound() const = 0;
        virtual bool Intersect(const Ray &r, SurfaceInteraction *) const = 0;
        virtual bool IntersectP(const Ray &r) const = 0;
        virtual void ComputeScatteringFunctions() const = 0;

};

class GeometricPrimitive : public Primitive {
    public:
        virtual Bounds3f WorldBound() const;
        virtual bool Intersect(const Ray &r, SurfaceInteraction *isect) const;
        virtual bool IntersectP(const Ray &r) const;
        GeometricPrimitive(const std::shared_ptr<Shape> &shape);
        void ComputeScatteringFunctions() const;

    private:
        std::shared_ptr<Shape> shape;
};

class Aggregate : public Primitive {
    public:
        void ComputeScatteringFunctions() const;
};

}