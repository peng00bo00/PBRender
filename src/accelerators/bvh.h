#pragma once

#include "PBRender.h"
#include "primitive.h"

namespace PBRender {

struct BVHBuildNode;

class BVHAccel : public Aggregate {
    public:
        BVHAccel(std::vector<std::shared_ptr<Primitive>> p,
                 int maxPrimsInNode = 1);
        
        ~BVHAccel();

        Bounds3f WorldBound() const;
        bool Intersect(const Ray &ray, SurfaceInteraction *isect) const;
        bool IntersectP(const Ray &ray) const;
    
    private:
        int flattenBVHTree(BVHBuildNode *node, int *offset);
    
    private:
        const int maxPrimsInNode;
        std::vector<std::shared_ptr<Primitive>> primitives;
        // LinearBVHNode *nodes = nullptr;
};

}