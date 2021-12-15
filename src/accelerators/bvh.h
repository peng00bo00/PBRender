#pragma once

#include "PBRender.h"
#include "primitive.h"

namespace PBRender {

struct BVHBuildNode;

struct BVHPrimitiveInfo;
struct MortonPrimitive;
struct LinearBVHNode;

class BVHAccel : public Aggregate {
    public:
        BVHAccel(std::vector<std::shared_ptr<Primitive>> p,
                 int maxPrimsInNode = 1);
        Bounds3f WorldBound() const;
        ~BVHAccel();
        bool Intersect(const Ray &ray, SurfaceInteraction *isect) const;
        bool IntersectP(const Ray &ray) const;
    
    private:
        BVHBuildNode *recursiveBuild(
            std::vector<BVHPrimitiveInfo> &primitiveInfo,
            int start, int end, int *totalNodes,
            std::vector<std::shared_ptr<Primitive>> &orderedPrims);
        int flattenBVHTree(BVHBuildNode *node, int *offset);
    
        const int maxPrimsInNode;
        std::vector<std::shared_ptr<Primitive>> primitives;
        LinearBVHNode *nodes = nullptr;
};

std::shared_ptr<BVHAccel> CreateBVHAccelerator(std::vector<std::shared_ptr<Primitive>> prims);

}